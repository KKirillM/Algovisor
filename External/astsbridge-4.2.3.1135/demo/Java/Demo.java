import com.micex.client.API.ServerInfo;
import com.micex.client.Binder;
import com.micex.client.Client;
import com.micex.client.ClientException;
import com.micex.client.Filler;
import com.micex.client.Meta;
import com.micex.client.Parser;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;

public class Demo implements Binder {

  public static void main(String[] args) throws ClientException {
    final Map<String,String> m = new HashMap<String, String>();
    m.put("PacketSize","60000");
    m.put("Interface","IFCBroker_20");
    m.put("Server","INET_GATEWAY");
    m.put("Service","inet_gateway");
    m.put("Broadcast","91.208.232.101");
    m.put("PrefBroadcast","91.208.232.101");
    m.put("UserID", "MU0000800001");
    m.put("Password", "");
    m.put("Language", "English");
    new Demo().run(m);
  }

  public void run(Map<String,String> parameters) throws ClientException {
    Client client = new Client();
    client.start(parameters);
    
    try {
      // Some useful info about connection
      System.out.println(String.format("Connected to MICEX, handle=%d", client.handle()));
      final ServerInfo info = client.getServerInfo();
      System.out.println(String.format("SystemID=%s; SessionID=%d; UserID=%s", 
        info.systemID, info.sessionID, info.userID));

      // Parsed market interface, contains meta-information
      // about available requests (tables) / transactions
      // and their structure definition.
      final Meta.Market market = client.getMarket();
      System.out.println(String.format("Market: %s", market.name));

      // Optional MTESelectBoards
      final Set<String> b = new HashSet<String>();
      b.add("TQBR");  // Use only one - limit number of SECURITIES in demo
      client.selectBoards(b);
      
      Parser parser;

      // load() table - mimics a sequence of MTEOpenTable/MTECloseTable.
      // Use it for non-updateable info-requests
      parser = client.load("MARKETS", null);
      parser.execute(this);

      // open() table - it will also be added to the list
      // of requests to be updated at refresh() call
      parser = client.open("TESYSTIME", null, true);
      parser.execute(this);
      
      // open() SECURITIES (params==null - all securities)
      parser = client.open("SECURITIES", null, false);
      parser.execute(this);
      
      // another (better) way to specify table name
      Meta.Message orderbooks = market.tables().find(Meta.TableType.Orderbooks);
      if (orderbooks != null) {
        parser = client.open(orderbooks.name, null, false);
        parser.execute(this);
      }

      // make 10 refresh() iterations with some delay between them
      for (int i = 0; i < 10; i++) {
        try { 
          Thread.sleep(1000); 
        } catch (InterruptedException e) {
          return;
        }
        parser = client.refresh();
        if (parser.empty()) continue; // nothing to parse, skip the rest
        int bytes = parser.length();
        int count = parser.execute(this);
        System.out.println("====================");
        System.out.println("Parsed " + bytes + " bytes, " + count + " rows");
      }
      
    } finally {
      client.close();
      System.out.println("====================");
      System.out.println("Done.");
    }
  }
  
  /**
   * Very simplistic table storage.
   */
  final Map<String, Table> database = new HashMap<String, Table>();
  
  /**
   * @param source - a TE request which is gonna be parsed
   * @return - a Filler instance which will be used to
   * store parsed values in some kind of storage or {@code null}
   * if not interested in storing parsed data.
   */
  public Filler getFiller(Meta.Message source) {
    // Of course, IRL you shouldn`t return a new instance
    // of Filler instance, but search some kind of 
    // internal database and return the same "table"
    // instance for every source message.
    Table table = database.get(source.name);
    if (table == null) {
      table = new Table();
      database.put(source.name, table);
    }
    return table;
  }

  /**
   * Very simplistic record.
   */
  static class Record {
    
    int decimals;
    
    final Map<String,Object> values = new LinkedHashMap<String, Object>();
    
  }
  
  /**
   * Very simplistic table.
   */
  static class Table implements Filler {

    /**
     * Записи, хэшированные по значению первичного ключа (если есть)
     * или по порядковому номеру при отсутствии ключевых полей.
     */
    final Map<String, Record> records = new HashMap<String, Record>();
    
    /**
     * Специальная структура для хранения "стаканов" - блоки записей,
     * хешированные по коду инструмента (SECBOARD + SECCODE).
     */
    final Map<String, List<Record>> orderbooks = new HashMap<String, List<Record>>();
    
    public boolean initTableUpdate(Meta.Message table) {
      // Парсер начинает обрабатывать буфер ответа.
      // Здесь также при необходимости нужно провести 
      // обработку в зависимости комбинации значений
      // table.isClearOnUpdate()  table.isOrderbook();
      if (table.isClearOnUpdate())
        records.clear();
      return true;  // Начиная с 1.1.0 возвращаемое значение игнорируется.
    }

    public void doneTableUpdate(Meta.Message table) {
      // Просто окончание работы парсера, а-ля commit.
      // Очистка более ненужных переменных.
      orderbook = null;
    }

    final Map<String, Object> keys = new LinkedHashMap<String, Object>();
    
    public void setKeyValue(Meta.Field field, Object value) {
      // Задает значения ключевых полей.
      // В демо-примере - собираем значения в map.
      // По окончании обработки записи собранные значения следует
      // сбросить (см. doneRecordUpdate())
      keys.put(field.name, value);
    }

    Record current;
    
    List<Record> orderbook;
    
    public boolean initRecordUpdate(Meta.Message table) {
      // Начало обработки записи.
      // Здесь нужно произвести поиск по ключам
      // и вернуть true, если запись не найдена (т.е. новая)
      if (table.isOrderbook()) {
        // Для таблиц типа "orderbook" - специальная обработка
        // Запись всегда будет новой, добавим ее в "стакан"
        current = new Record();
        orderbook.add(current);
        return true;
      } else {
        System.out.println("Table:" + table.name +"; keys: " + keys.toString());
        if (keys.isEmpty()) {
          // setKeyValue() ни разу не был вызван - 
          // данная таблица не имеет первичных ключей.
          // Все записи будут считаться новыми, хранить
          // их будем по порядковому номеру.
          current = new Record();
          records.put(Integer.toString(records.size()), current);
          return true;
        } else {
          // У таблицы есть первичные ключи - 
          // ищем и храним записи по их значению.
          final String key = keys.toString();
          current = records.get(key);
          if (current == null) {
            // Запись по ключу не найдена - создадим новую
            current = new Record();
            records.put(key, current);
            return true;
          }
          // Запись была найдена
          return false;
        }
      }
    }

    public void setRecordDecimals(int decimals) {
      // Парсер дли новой записи определил 
      // кол-во десятичных знаков - сохранить (в записи).
      current.decimals = decimals;
    }

    public int getRecordDecimals() {
      // Парсеру для работы понадобилось знать
      // о кол-ве десятичных знаков по найденной 
      // ранее записи - отдать сохраненное.
      return current.decimals;
    }

    public void setFieldValue(Meta.Field field, Object value) {
      // Парсер задает значение конкретного поля записи
      current.values.put(field.name, value);
    }

    public void doneRecordUpdate(Meta.Message table) {
      // Закончили обрабатывать запись.
      // Хорошее место для сохранения накопленных данных куда-то.
      // В демо-примере - просто печать в консоль.
      System.out.println("Table:" + table.name +"; data: " + current.values.toString());
      // Здесь также следует сбросить собранные значения первичных ключей,
      // чтобы подготовиться к получению значений для слудеющей записи.
      keys.clear();
      current = null;
    }

    public void switchOrderbook(Meta.Message table, Meta.Ticker ticker) {
      // Специфическая операция для таблиц типа "котировки" (table.isOrderbook())
      // Для таких таблиц значения ключевых полей setKeyField() не задаются для 
      // каждой записи, вместо этого идет "переключение стакана" - блока записей.
      // Данный вызов информирует вас о том, что начинается новый блок
      // для указанного инструмента ticker.
      orderbook = orderbooks.get(ticker.toString());
      if (orderbook == null) {
        // Инструмент встретился впервые,
        // подготовм для него "стакан"
        orderbook = new ArrayList<Record>();
        orderbooks.put(ticker.toString(), orderbook);
      } else {
        // "Стакан" уже есть - его нужно очистить,
        // т.к. новые значения полностью заменяют старые.
        orderbook.clear();
      }
    }
    
  }
  
}
