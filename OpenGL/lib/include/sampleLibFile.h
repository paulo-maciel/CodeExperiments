#pragma once

class IThirdEye {
  public:
  virtual ~IThirdEye();
  virtual void uninit() = 0;
};

class ThirdEye : public IThirdEye {
  public:

  ~ThirdEye();
  void uninit() override;
};

