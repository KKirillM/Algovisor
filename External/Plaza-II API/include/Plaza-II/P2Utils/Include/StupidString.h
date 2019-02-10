#ifndef _STUPID_STRING_H
#define _STUPID_STRING_H

class stupid_string
{
public:
	stupid_string(const char *s = NULL, bool deleteString = true) : m_str(s), m_delete(deleteString)
	{
		if (deleteString)
		{
			copy_from(s);
		}
	}

	stupid_string(const stupid_string& str) : m_str(str.m_str), m_delete(str.m_delete)
	{
		if (m_delete)
		{
			copy_from(str.m_str);
		}
	}

	~stupid_string()
	{
		if (m_delete)
		{
			delete [] const_cast<char *>(m_str);
		}
	}

	stupid_string& operator = (const stupid_string& str)
	{
		if (this != &str)
		{
			if (m_delete)
			{
				delete [] const_cast<char *>(m_str);
			}
			m_delete = str.m_delete;
			copy_from(str.m_str);
		}
		return *this;
	}

	bool operator < (const stupid_string& str)const
	{
		return strcmp(m_str, str.m_str) < 0;
	}

private:
	void copy_from(const char *s)
	{
		if (s == NULL)
		{
			m_str = NULL;
			return;
		}
		m_str = new char[strlen(s) + 1];
		strcpy(const_cast<char *>(m_str), s);
	}
public:
	const char * m_str;
	bool m_delete;
};

#endif

