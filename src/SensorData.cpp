class SensorData
{
private:
  float *data1;
  float *data2;
  float *data3;
  int capacity;
  int count1;
  int count2;
  int count3;

public:
  SensorData(int initialCapacity = 10)
  {
    capacity = initialCapacity;
    count1 = 0;
    count2 = 0;
    count3 = 0;
    data1 = new float[capacity];
    data2 = new float[capacity];
    data3 = new float[capacity];
  }

  ~SensorData()
  {
    delete[] data1;
    delete[] data2;
    delete[] data3;
  }

  bool add1(float value)
  {
    if (count1 >= capacity)
    {
      int newCapacity = capacity * 2;
      float *newData = new float[newCapacity];

      if (newData == nullptr)
      {
        return false;
      }

      for (int i = 0; i < count1; i++)
      {
        newData[i] = data1[i];
      }

      delete[] data1;
      data1 = newData;
      capacity = newCapacity;
    }

    data1[count1] = value;
    count1++;
    return true;
  }

  bool add2(float value)
  {
    if (count2 >= capacity)
    {
      int newCapacity = capacity * 2;
      float *newData = new float[newCapacity];

      if (newData == nullptr)
      {
        return false;
      }

      for (int i = 0; i < count2; i++)
      {
        newData[i] = data2[i];
      }

      delete[] data2;
      data2 = newData;
      capacity = newCapacity;
    }

    data2[count2] = value;
    count2++;
    return true;
  }

  bool add3(float value)
  {
    if (count3 >= capacity)
    {
      int newCapacity = capacity * 2;
      float *newData = new float[newCapacity];

      if (newData == nullptr)
      {
        return false;
      }

      for (int i = 0; i < count3; i++)
      {
        newData[i] = data3[i];
      }

      delete[] data3;
      data3 = newData;
      capacity = newCapacity;
    }

    data3[count3] = value;
    count3++;
    return true;
  }

  bool remove1(int index)
  {
    if (index < 0 || index >= count1)
    {
      return false;
    }

    for (int i = index; i < count1 - 1; i++)
    {
      data1[i] = data1[i + 1];
    }

    count1--;
    return true;
  }

  bool remove2(int index)
  {
    if (index < 0 || index >= count2)
    {
      return false;
    }

    for (int i = index; i < count2 - 1; i++)
    {
      data2[i] = data2[i + 1];
    }

    count2--;
    return true;
  }

  bool remove3(int index)
  {
    if (index < 0 || index >= count3)
    {
      return false;
    }

    for (int i = index; i < count3 - 1; i++)
    {
      data3[i] = data3[i + 1];
    }

    count3--;
    return true;
  }

  void clear1()
  {
    count1 = 0;
  }

  void clear2()
  {
    count2 = 0;
  }

  void clear3()
  {
    count3 = 0;
  }

  void clearAll()
  {
    count1 = 0;
    count2 = 0;
    count3 = 0;
  }

  float get1(int index)
  {
    if (index >= 0 && index < count1)
    {
      return data1[index];
    }
    return 0.0;
  }

  float get2(int index)
  {
    if (index >= 0 && index < count2)
    {
      return data2[index];
    }
    return 0.0;
  }

  float get3(int index)
  {
    if (index >= 0 && index < count3)
    {
      return data3[index];
    }
    return 0.0;
  }

  int size1()
  {
    return count1;
  }

  int size2()
  {
    return count2;
  }

  int size3()
  {
    return count3;
  }
};