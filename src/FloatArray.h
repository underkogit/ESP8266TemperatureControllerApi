class FloatArray
{
private:
  float *shtData;
  float *ahtData;
  float *bmpData;
  int capacity;
  int shtCount;
  int ahtCount;
  int bmpCount;

public:
  FloatArray(int initialCapacity = 10)
  {
    capacity = initialCapacity;
    shtCount = 0;
    ahtCount = 0;
    bmpCount = 0;
    shtData = new float[capacity];
    ahtData = new float[capacity];
    bmpData = new float[capacity];
  }

  ~FloatArray()
  {
    delete[] shtData;
    delete[] ahtData;
    delete[] bmpData;
  }

  bool addSHT(float value)
  {
    if (shtCount >= capacity)
    {
      int newCapacity = capacity * 2;
      float *newData = new float[newCapacity];

      if (newData == nullptr)
      {
        return false;
      }

      for (int i = 0; i < shtCount; i++)
      {
        newData[i] = shtData[i];
      }

      delete[] shtData;
      shtData = newData;
      capacity = newCapacity;
    }

    shtData[shtCount] = value;
    shtCount++;
    return true;
  }

  bool addAHT(float value)
  {
    if (ahtCount >= capacity)
    {
      int newCapacity = capacity * 2;
      float *newData = new float[newCapacity];

      if (newData == nullptr)
      {
        return false;
      }

      for (int i = 0; i < ahtCount; i++)
      {
        newData[i] = ahtData[i];
      }

      delete[] ahtData;
      ahtData = newData;
      capacity = newCapacity;
    }

    ahtData[ahtCount] = value;
    ahtCount++;
    return true;
  }

  bool addBMP(float value)
  {
    if (bmpCount >= capacity)
    {
      int newCapacity = capacity * 2;
      float *newData = new float[newCapacity];

      if (newData == nullptr)
      {
        return false;
      }

      for (int i = 0; i < bmpCount; i++)
      {
        newData[i] = bmpData[i];
      }

      delete[] bmpData;
      bmpData = newData;
      capacity = newCapacity;
    }

    bmpData[bmpCount] = value;
    bmpCount++;
    return true;
  }

  bool removeSHT(int index)
  {
    if (index < 0 || index >= shtCount)
    {
      return false;
    }

    for (int i = index; i < shtCount - 1; i++)
    {
      shtData[i] = shtData[i + 1];
    }

    shtCount--;
    return true;
  }

  bool removeAHT(int index)
  {
    if (index < 0 || index >= ahtCount)
    {
      return false;
    }

    for (int i = index; i < ahtCount - 1; i++)
    {
      ahtData[i] = ahtData[i + 1];
    }

    ahtCount--;
    return true;
  }

  bool removeBMP(int index)
  {
    if (index < 0 || index >= bmpCount)
    {
      return false;
    }

    for (int i = index; i < bmpCount - 1; i++)
    {
      bmpData[i] = bmpData[i + 1];
    }

    bmpCount--;
    return true;
  }

  void clearSHT()
  {
    shtCount = 0;
  }

  void clearAHT()
  {
    ahtCount = 0;
  }

  void clearBMP()
  {
    bmpCount = 0;
  }

  void clearAll()
  {
    shtCount = 0;
    ahtCount = 0;
    bmpCount = 0;
  }

  float getSHT(int index)
  {
    if (index >= 0 && index < shtCount)
    {
      return shtData[index];
    }
    return 0.0;
  }

  float getAHT(int index)
  {
    if (index >= 0 && index < ahtCount)
    {
      return ahtData[index];
    }
    return 0.0;
  }

  float getBMP(int index)
  {
    if (index >= 0 && index < bmpCount)
    {
      return bmpData[index];
    }
    return 0.0;
  }

  int sizeSHT()
  {
    return shtCount;
  }

  int sizeAHT()
  {
    return ahtCount;
  }

  int sizeBMP()
  {
    return bmpCount;
  }
};