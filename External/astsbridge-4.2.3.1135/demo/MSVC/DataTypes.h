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

// ���� ��� ������ � �������� MTEOpenTable() � MTERefresh()

struct TMTERow
{
    char fieldCount;        // ���-�� ����� //���� �������� ����� 0, � ������ ������������ ��� ���� ������� (��. MTEStructure)
    int dataLength;         // ����� ������
    char* fieldNumbers;     // ��������� �� ������ �����, ������ ����� ����� ��� char (�� ������ ����� �� �����)
    char* fieldData;        // ��������� �� ������ �����

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
    int ref;                /* ������������ ��� ������� ��������� ����� �� ���������� �������� � ������� ������� MTEAddTable/MTERefresh
                               ��� �������� ��������, ���������� � �������� �������� ��������� ������� MTEAddTable
                               �� �������� ����� ���� ����� ����������, ����� ������� (���������� HTable) ������������� ���������� ��������� TMTETable. 
                               � ������, ������������ MTEOpenTable, �������� ���� "Ref" �� ����������. */

    int rowsCount;          // ���-�� �����
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
    int tablesCount;        // ���-�� ������
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


// ���� ��� ������ � �������� MTEStructureEx()

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
    ffKey           = 0x01,     // ���� �������� ��������. ������ ������� � ������������ ���������� �������� ����� ������ ������������ � ���� ������.
    ffSecCode       = 0x02,     // ���� �������� ��� ����������� �����������. ������������� ��������� ������ ���� ��� ������������� ��������� ����������� ����� ������ ����� ������� � �������� ����� ���� FLOAT.
    ffNotNull       = 0x04,     // ���� �� ����� ���� ������.
    ffVarBlock      = 0x08      // ���� ������ � ������ �����, ������� ����� ����������� ��������� ���.
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
    tfNonUpdateable = 0,    // ������� �� ����������� ��� �� ����� ������� ������ ������� MTEOpenTable
    tfUpdateable = 1,       // ������� �������� �����������. ��� ��� ����� �������� ������� MTEAddTable/MTERefresh
    tfClearOnUpdate = 2,    // ������ ���������� ������� ������ ��������� ��� ��������� ������� ���������� � ������� ������� MTEAddTable/MTERefresh
    tfOrderbook = 4         // ������� ����� ������ ��������� � ������ �������������� �������������� ������� (��. ��������� �� ������ � ���������)
};

struct TTable
{
    String name;
    String header;
    String description;
    int index;              // ���� ��������������� �������� ����� ���������� �������� �������, ������� ������������ ������ ������. 
                            // ����� ����������, ����������� �������� MTEAddTable, ����� ��������� ������ ������� � ���������� �����������������. 
                            // � ��������� ����� �� ���� ������, ����� ��������, ���� ������ ����� 0, � ��� ������� ����� ����������� ����� ������� MTERefresh. 
                            // �� ������� ����� �������� ��� ����������: ���������� �������� � ������� ����-����������� � ������� ��� ������� �� ���������� ������ 
                            // ����������� �� ��� ������ � ������������ � ��������� ��������.
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
    char account[12];           // �������� ����
    char buySell;               // �������������� ������: 'B' - �����, 'S' - �������
    char mktLimit;              // ��� ������: 'M'- ��������, 'L'- ��������, 'C'- ������ ������� ��������
    char splitFlag;             // ������� ����������� ����: 'S' - �� ������ �����, 'O' - �� ����� ����
    char immCancel;             // ������� ����������: ' ' - ��������� � �������, 'N' - ��������� ��� ���������, 'W' - ����� �������
    char priceYieldEnterType;   // ������� ����� �������� ����: 'P'- �� ����, 'Y'- �� ����������, 'W'- �� ���������������� ����
    char mmOrder;               // ���������� ������: ' '- �� ���������, 'M'- ������ ������-�������, 'S'- ������ ����������� ����
    char activationType;        // ��� ������������� ������ �������: ' '- ������� ������, 'T'- ������ � �������� ���������, 'C'- ������ � ������� ��������
    char secboard[4];           // ����� ������
    char seccode[12];           // ����������
    char price[9];              // ����
    char quantity[10];          // ����
    char hidden[10];            // ������ �����
    char brokerref[20];         // ����������
    char orderValue[16];        // ����� ������
    char extRef[12];            // ��� �������� ������������
    char activationTime[6];     // ����� ���������
    char clientCode[12];        // ��� �������
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