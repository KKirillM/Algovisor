#pragma once

extern bool g_isVersion2;

#define MTE_VERSION_NEW bool g_isVersion2 = true;
#define MTE_VERSION_OLD bool g_isVersion2 = false;

struct String
{
    int length;
    char* data;

    void SetValues(char*& ptr)
    {
        length = *(int*)ptr;
        ptr += sizeof(length);
        data = ptr;
        ptr += length;
    }

    size_t GetSize()
    {
        return sizeof(length) + length;
    }
};

// Типы для работы с функцией MTEOpenTable() и MTERefresh()

struct TMTERow
{
    char fieldCount;        // кол-во полей //если значение равно 0, в строке присутствуют все поля таблицы (см. MTEStructure)
    int dataLength;         // длина данных
    char* fieldNumbers;     // указывает на номера полей, каждый номер имеет тип char (по одному байту на номер)
    char* fieldData;        // указывает на данные полей

    void SetValues(char*& ptr)
    {
        fieldCount = *(char*)ptr; ptr += sizeof(fieldCount);
        dataLength = *(int*)ptr; ptr += sizeof(dataLength);
        if (fieldCount)   
        {
            fieldNumbers = (char*)ptr; ptr += fieldCount;
        }
        fieldData = (char*)ptr; ptr += dataLength;
    }

    size_t GetSize()
    {
        return (sizeof(fieldCount) + sizeof(dataLength) + fieldCount + dataLength);
    }
};

struct TMTETable
{
    int ref;                /* используется при запросе изменений сразу по нескольким таблицам с помощью функций MTEAddTable/MTERefresh
                               Оно содержит значение, переданное в качестве третьего параметра функции MTEAddTable
                               По значению этого поля можно определить, какой таблице (дескриптор HTable) соответствует полученная структура TMTETable. 
                               В буфере, возвращаемом MTEOpenTable, значение поля "Ref" не определено. */

    int rowsCount;          // кол-во строк
    TMTERow* rows;

    void SetValues(char*& ptr)
    {
        ref = *(int*)ptr; ptr += sizeof(ref);
        rowsCount = *(int*)ptr; ptr += sizeof(rowsCount);
        rows = (TMTERow*)ptr;
        ptr = SetPointerToRow(rowsCount);
    }

    char* SetPointerToRow(const int num)
    {
        char* ptr = (char*)rows;
        int lastValue = num >= rowsCount ? rowsCount : num;
        for (int i = 0; i < lastValue; ++i)
        {
            TMTERow row;
            row.SetValues(ptr);
        }
        return ptr;
    }

    size_t GetSize()
    {
        char* ptr = (char*)rows;
        for (int i = 0; i < rowsCount; ++i)
        {
            TMTERow row;
            row.SetValues(ptr);
        }
        return ptr-(char*)rows;
    }
};

struct TMTETables
{
    int tablesCount;        // кол-во таблиц
    TMTETable* tables;

    void SetValues(char*& ptr)
    {
        tablesCount = *(int*)ptr; ptr += sizeof(tablesCount);
        tables = (TMTETable*)ptr;
        //ptr = SetPointerToTable(tablesCount);
    }

    char* SetPointerToTable(const int num)
    {
        char* ptr = (char*)tables;
        int lastValue = num >= tablesCount ? tablesCount : num;
        for (int i = 0; i < lastValue; ++i)
        {
            TMTETable table;
            table.SetValues(ptr);
        }
        return ptr;
    }
};


// Типы для работы с функцией MTEStructureEx()

enum class TEnumKind
{
    ekCheck = 0,
    ekGroup = 1,
    ekCombo = 2
};

struct TEnumConst
{
    String value;
    String longDescription;
    String shortDescription;

    void SetValues(char*& ptr)
    {
        value.SetValues(ptr);
        if (g_isVersion2)
        {
            longDescription.SetValues(ptr);
            shortDescription.SetValues(ptr);
        }
    }

    size_t GetSize()
    {
        return value.GetSize() + longDescription.GetSize() + shortDescription.GetSize();
    }
};

struct TEnumType
{
    String name;
    String header;
    String description;
    int size;
    TEnumKind kind;
    int numberOfConstatns;
    TEnumConst* consts;

    void SetValues(char*& ptr)
    {
        name.SetValues(ptr);
        header.SetValues(ptr);
        if (g_isVersion2)
            description.SetValues(ptr);
        size = *(int*)ptr; ptr += sizeof(size);
        kind = *(TEnumKind*)ptr; ptr += sizeof(TEnumKind);
        numberOfConstatns = *(int*)ptr; ptr += sizeof(numberOfConstatns);
        consts = (TEnumConst*)ptr;
        ptr = SetPointerToEnumConst(numberOfConstatns);
    }

    char* SetPointerToEnumConst(const int num)
    {
        char* ptr = (char*)consts;
        int lastValue = num >= numberOfConstatns ? numberOfConstatns : num;
        for (int i = 0; i < lastValue; ++i)
        {
            TEnumConst e;
            e.SetValues(ptr);
        }
        return ptr;
    }

    size_t GetSize()
    {
        char* ptr = (char*)consts;
        for (int i = 0; i < numberOfConstatns; ++i)
        {
            TEnumConst ec;
            ec.SetValues(ptr);
        }

        return name.GetSize() + header.GetSize() + description.GetSize() + sizeof(size) + sizeof(kind) + sizeof(numberOfConstatns) + (ptr - (char*)consts);
    }
};

struct TEnumTypes
{
    int numberOfTypes;
    TEnumType* type;

    void SetValues(char*& ptr)
    {
        numberOfTypes = *(int*)ptr;
        ptr += sizeof(numberOfTypes);
        type = (TEnumType*)ptr;
        ptr = SetPointerToValue(numberOfTypes);
    }

    char* SetPointerToValue(const int num)
    {
        char* ptr = (char*)type;
        int lastValue = num >= numberOfTypes ? numberOfTypes : num;
        for (int i = 0; i < lastValue; ++i)
        {
            TEnumType t;
            t.SetValues(ptr);
        }
        return ptr;
    }

    size_t GetSize()
    {
        char* ptr = SetPointerToValue(numberOfTypes);
        size_t enumTypeSize = ptr - (char*)type;

        return sizeof(numberOfTypes) + enumTypeSize;
    }
};

enum class TFieldType
{
    ftChar          = 0,
    ftInteger       = 1,
    ftFixed         = 2,
    ftFloat         = 3,
    ftDate          = 4,
    ftTime          = 5,
    ftFloatPoint    = 6
};

enum class TFieldFlags
{
    ffKey           = 0x01,     // Поле является ключевым. Строки таблицы с совпадающими значениями ключевых полей должны объединяться в одну строку.
    ffSecCode       = 0x02,     // Поле содержит код финансового инструмента. Рекомендуется учитывать данный флаг при автоматизации процедуры определения числа знаков после запятой в числовых полях типа FLOAT.
    ffNotNull       = 0x04,     // Поле не может быть пустым.
    ffVarBlock      = 0x08      // Поле входит в группу полей, которые могут повторяться несколько раз.
};

struct TFieldOut
{
    String name;
    String header;
    String description;
    int size;
    TFieldType type;
    int numberOfDecimalPlaces;
    TFieldFlags attributes;
    String enumType;

    void SetValues(char*& ptr)
    {
        name.SetValues(ptr);
        header.SetValues(ptr);
        if (g_isVersion2)
            description.SetValues(ptr);
        size = *(int*)ptr; ptr += sizeof(size);
        type = *(TFieldType*)ptr; ptr += sizeof(TFieldType);
        if (g_isVersion2)
        {
            numberOfDecimalPlaces = *(int*)ptr; ptr += sizeof(numberOfDecimalPlaces);
        }
        attributes = *(TFieldFlags*)ptr; ptr += sizeof(TFieldFlags);
        enumType.SetValues(ptr);
    }
};

struct TFieldIn
{
    String name;
    String header;
    String description;
    int size;
    TFieldType type;
    int numberOfDecimalPlaces;
    TFieldFlags attributes;
    String enumType;
    String defaultValue;

    void SetValues(char*& ptr)
    {
        name.SetValues(ptr);
        header.SetValues(ptr);
        if (g_isVersion2)
            description.SetValues(ptr);
        size = *(int*)ptr; ptr += sizeof(size);
        type = *(TFieldType*)ptr; ptr += sizeof(TFieldType);
        if (g_isVersion2)
        {
            numberOfDecimalPlaces = *(int*)ptr; ptr += sizeof(numberOfDecimalPlaces);
        }
        attributes = *(TFieldFlags*)ptr; ptr += sizeof(TFieldFlags);
        enumType.SetValues(ptr);
        defaultValue.SetValues(ptr);
    }
};

struct TFieldsOut
{
    int numberOfFields;
    TFieldOut* fields;

    void SetValues(char*& ptr)
    {
        numberOfFields = *(int*)ptr; ptr += sizeof(numberOfFields);
        fields = (TFieldOut*)ptr;
        ptr = SetPointerToValue(numberOfFields);
    }

    char* SetPointerToValue(const int num)
    {
        char* ptr = (char*)fields;
        int lastValue = num >= numberOfFields ? numberOfFields : num;
        for (int i = 0; i < lastValue; ++i)
        {
            TFieldOut field;
            field.SetValues(ptr);
        }
        return ptr;
    }

    size_t GetSize()
    {
        char* ptr = SetPointerToValue(numberOfFields);
        size_t fieldTypeSize = ptr - (char*)fields;
        return sizeof(numberOfFields) + fieldTypeSize;
    }
};

struct TFieldsIn
{
    int numberOfFields;
    TFieldIn* fields;

    void SetValues(char*& ptr)
    {
        numberOfFields = *(int*)ptr; ptr += sizeof(numberOfFields);
        fields = (TFieldIn*)ptr;
        ptr = SetPointerToValue(numberOfFields);
    }

    char* SetPointerToValue(const int num)
    {
        char* ptr = (char*)fields;
        int lastValue = num >= numberOfFields ? numberOfFields : num;
        for (int i = 0; i < lastValue; ++i)
        {
            TFieldIn t;
            t.SetValues(ptr);
        }
        return ptr;
    }

    size_t GetSize()
    {
        char* ptr = SetPointerToValue(numberOfFields);
        size_t fieldTypeSize = ptr - (char*)fields;
        return sizeof(numberOfFields) + fieldTypeSize;
    }
};

enum class TTableFlags
{
    tfNonUpdateable = 0,    // таблица не обновляемая для неё можно вызвать только функцию MTEOpenTable
    tfUpdateable = 1,       // таблица является обновляемой. Для нее можно вызывать функции MTEAddTable/MTERefresh
    tfClearOnUpdate = 2,    // старое содержимое таблицы должно удаляться при получении каждого обновления с помощью функций MTEAddTable/MTERefresh
    tfOrderbook = 4         // таблица имеет формат котировок и должна обрабатываться соответсвующим образом (см. Замечания по работе с таблицами)
};

struct TTable
{
    String name;
    String header;
    String description;
    int index;              // Поле «ИндексСистемы» содержит номер подсистемы Торговой системы, которая обрабатывает данный запрос. 
                            // Пакет обновлений, формируемый вызовами MTEAddTable, может содержать только запросы с одинаковым «ИндексомСистемы». 
                            // В настоящее время на всех рынках, кроме срочного, этот индекс равен 0, и все таблицы могут обновляться одним вызовом MTERefresh. 
                            // На срочном рынке работают две подсистемы: собственно торговая и система риск-менеджмента – поэтому все запросы на обновление должны 
                            // разбиваться на два пакета в соответствие с «Индексом системы».
    TTableFlags attributes;
    TFieldsIn inputFiedls;
    TFieldsOut outputFiedls;

    void SetValues(char*& ptr)
    {
        name.SetValues(ptr);
        header.SetValues(ptr);
        if (g_isVersion2)
        {
            description.SetValues(ptr);
            index = *(int*)ptr; ptr += sizeof(index);
        }
        attributes = *(TTableFlags*)ptr; ptr += sizeof(TTableFlags);
        inputFiedls.SetValues(ptr);
        outputFiedls.SetValues(ptr);
    }
};

struct TTables
{
    int tablesCount;
    TTable* tables;

    void SetValues(char*& ptr)
    {
        tablesCount = *(int*)ptr; ptr += sizeof(tablesCount);
        tables = (TTable*)ptr;
        ptr = SetPointerToValue(tablesCount);
    }

    char* SetPointerToValue(const int num)
    {
        char* ptr = (char*)tables;
        int lastValue = num >= tablesCount ? tablesCount : num;
        for (int i = 0; i < lastValue; ++i)
        {
            TTable t;
            t.SetValues(ptr);
        }
        return ptr;
    }

    TTable GetTableByName(std::string& name)
    {
        char* ptr = (char*)tables;
        TTable table;
        for (int i = 0; i < tablesCount; ++i)
        {
            table.SetValues(ptr);
            std::string orderbookTable(table.name.data, table.name.length);
            if (orderbookTable == name)
            {
                break;
            }
        }

        return table;
    }

    size_t GetSize()
    {
        char* ptr = SetPointerToValue(tablesCount);
        size_t tableTypeSize = ptr - (char*)tables;
        return sizeof(tablesCount) + tableTypeSize;
    }
};

struct TTransaction
{
    String name;
    String header;
    String description;
    int index;
    TFieldsIn inputFields;

    void SetValues(char*& ptr)
    {
        name.SetValues(ptr);
        header.SetValues(ptr);
        if (g_isVersion2)
        {
            description.SetValues(ptr);
            index = *(int*)ptr; ptr += sizeof(index);
        }
        inputFields.SetValues(ptr);
    }

    size_t GetSize()
    {
        return name.GetSize() + header.GetSize() + description.GetSize() + sizeof(index) + inputFields.GetSize();
    }
};

struct TTransactions
{
    int numberOfTransations;
    TTransaction* transactions;

    void SetValues(char*& ptr)
    {
        numberOfTransations = *(int*)ptr; ptr += sizeof(numberOfTransations);
        transactions = (TTransaction*)ptr;
        ptr = SetPointerToValue(numberOfTransations);
    }

    char* SetPointerToValue(const int num)
    {
        char* ptr = (char*)transactions;
        int lastValue = num >= numberOfTransations ? numberOfTransations : num;
        for (int i = 0; i < lastValue; ++i)
        {
            TTransaction t;
            t.SetValues(ptr);
        }
        return ptr;
    }

    TTransaction* GetTransationByName(const std::string& transactionName)
    {
        char* ptr1 = (char*)transactions;
        char* ptr2 = ptr1;
        for (int i = 0; i < numberOfTransations; ++i)
        {
            TTransaction t;
            t.SetValues(ptr1);
            if (std::string(t.name.data, t.name.length) == transactionName)
            {
                break;
            }
            ptr2 = ptr1;
        }
        return (TTransaction*)ptr2;
    }

    size_t GetSize()
    {
        char* ptr = SetPointerToValue(numberOfTransations);
        size_t tableTypeSize = ptr - (char*)transactions;
        return sizeof(numberOfTransations) + tableTypeSize;
    }
};

struct TInterface
{
    String interfaceName;
    String headerName;
    String description;
    TEnumTypes enumTypes;
    TTables tables;
    TTransactions transactions;

    char* interfaceData;

    TInterface() : interfaceData(NULL) {}

    void setData(char* data, int32_t len)
    {
        interfaceData = new char[len];
        memcpy(interfaceData, data, len);

        char* ptr = interfaceData;
        interfaceName.SetValues(ptr);
        headerName.SetValues(ptr);
        if (g_isVersion2)
            description.SetValues(ptr);
        enumTypes.SetValues(ptr);
        tables.SetValues(ptr);
        transactions.SetValues(ptr);
    }

    ~TInterface() { if (interfaceData) { delete[] interfaceData; } }

};

struct MTEMessage
{
    int32_t  dataLen;
    char*    data;

    MTEMessage() : dataLen(0), data(NULL) {}

    void set(MTEMSG* msg)
    {
        dataLen = msg->DataLen;
        data = new char[msg->DataLen];
    }

    ~MTEMessage()
    {
        if(data)
        {
            delete[] data;
        }
    }
};

struct OrderTransaction
{
    char account[12];           // Торговый счёт
    char buySell;               // Направленность заявки: 'B' - купля, 'S' - продажа
    char mktLimit;              // Тип заявки: 'M'- рыночная, 'L'- лимитная, 'C'- заявка периода закрытия
    char splitFlag;             // Признак расщепления цены: 'S' - по разным ценам, 'O' - по одной цене
    char immCancel;             // Условие исполнения: ' ' - поставить в очередь, 'N' - полностью или отклонить, 'W' - снять остаток
    char priceYieldEnterType;   // Признак ввода значения цены: 'P'- по цене, 'Y'- по доходности, 'W'- по средневзвешенной цене
    char mmOrder;               // Назначение заявки: ' '- по умолчанию, 'M'- заявка маркет-мейкера, 'S'- заявка поддержания цены
    char activationType;        // Тип активирующего заявку события: ' '- обычная заявка, 'T'- заявка с временем активации, 'C'- заявка в аукцион закрытия
    char secboard[4];           // Режим торгов
    char seccode[12];           // Инструмент
    char price[9];              // Цена
    char quantity[10];          // Лоты
    char hidden[10];            // Скрыто лотов
    char brokerref[20];         // Примечание
    char orderValue[16];        // Объем заявки
    char extRef[12];            // Код внешнего пользователя
    char activationTime[6];     // Время активации
    char clientCode[12];        // Код клиента
};

//struct ServInfo
//{
//    int Connected_To_Micex;
//    int Session_Id;
//    char MICEX_Sever_Name[33];
//    char Version_Major;
//    char Version_Minor;
//    char Version_Build;
//    char Beta_version;
//    char Debug_flag;
//    char Test_flag;
//    int Start_Time;
//    int Stop_Time_Min;
//    int Stop_Time_Max;
//    int Next_Event;
//    int Event_Date;
//    char BoardsSelected;
//    char UserID[13];
//    char SystemId;
//    char ServerIp;
//};