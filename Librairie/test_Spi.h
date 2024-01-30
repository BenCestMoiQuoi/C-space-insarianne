class SPI {
  protected:
    SPI();
    void SendSPI(uint8_t buff);
    uint8_t RecSPI(void);

    void begin(uint8_t cs_pin);



  private:
    uint8_t _CS_pin;

};
