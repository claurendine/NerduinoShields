--------------------------------------------------------------------------------
-- HMC5883 I2C module for NODEMCU
-- LICENCE: http://opensource.org/licenses/MIT
-- Chase <chase@nerduino.com>
--------------------------------------------------------------------------------
    local moduleName = ... 
    local M = {}
    _G[moduleName] = M
  
    --default value for i2c communication
    local id=0

    local readAddress = 0x3d
    local writeAddress = 0x3c
    local gainValue = 0;

    --default oversampling setting
    local oss = 0
    
    --CO: calibration coefficients table.
    local CO = {}

    -- read reg for 1 byte
    local function read_reg(reg_addr)
      i2c.start(id)
      i2c.address(id, readAddress, i2c.TRANSMITTER)
      i2c.write(id, reg_addr)
      i2c.stop(id)
      i2c.start(id)
      i2c.address(id, readAddress, i2c.RECEIVER)
      local c = i2c.read(id, 1)
      i2c.stop(id)
      return c
    end   

    --write reg for 1 byte
    local function write_reg(reg_addr, reg_val)
      i2c.start(id)
      i2c.address(id, writeAddress, i2c.TRANSMITTER)
      i2c.write(id, reg_addr)
      i2c.write(id, reg_val)
      i2c.stop(id)
    end    

    --get signed or unsigned 16
    --parameters:
    --reg_addr: start address of short
    --signed: if true, return signed16
    local function getShort(reg_addr, signed)
      local tH = string.byte(read_reg(reg_addr))
      local tL = string.byte(read_reg((reg_addr + 1)))
      local temp = tH*256 + tL
      if (temp > 32767) and (signed == true) then 
        temp = temp - 65536
      end
      return temp
    end    

    -- initialize i2c
    --parameters:
    --d: sda
    --l: scl
    function M.init(d, l)      
      if (d ~= nil) and (l ~= nil) and (d >= 0) and (d <= 11) and (l >= 0) and ( l <= 11) and (d ~= l) then
        sda = d
        scl = l 
      else 
        print("iic config failed!") return nil
      end
        print("init done")
        i2c.setup(id, sda, scl, i2c.SLOW)

        enableMagnetometer();
        setMagneticGain(1);

    end

    -- enable the magnetometer
    function M.enableMagnetometer()
        write_reg(0x02, 0x00); -- mode register
    end


    --set the magnetic gain
    --parameters:
    --num: 0 to 7
    function M.setMagneticGain(gain)
      if (gain >=0) and (gain <= 7) then
        write_reg(0x01, gain);

        if (gain == 0) then
            gainValue = 1370;
        else if (gain == 1) then
            gainValue = 1090;
        else if (gain == 2) then
            gainValue = 820;
        else if (gain == 3) then
            gainValue = 660;
        else if (gain == 4) then
            gainValue = 440;
        else if (gain == 5) then
            gainValue = 390;
        else if (gain == 6) then
            gainValue = 330;
        else if (gain == 7) then
            gainValue = 230;
        end
      end
    end

    -- read data
    function M.readData()
        -- send a request for measurement


        -- wait for 6 bytes to be available
    end

    --get x-axis value
    function M.getRawX()
        local msb = read_reg(0x03);
        local lsb = read_reg(0x04);

        local xaxis = msb * 256 + lsb;

        return xaxis;
    end

    --get y-axis value
    function M.getRawY()
        local msb = read_reg(0x07);
        local lsb = read_reg(0x08);

        local yaxis = msb * 256 + lsb;

        return yaxis;
    end

    --get z-axis value
    function M.getRawZ()
        local msb = read_reg(0x05);
        local lsb = read_reg(0x06);

        local zaxis = msb * 256 + lsb;

        return zaxis;
    end

    --get x-axis value
    function M.getX()
        local val = getRawX();
        local ret = val * 390.0 / gainValue;

        return ret;
    end

    --get y-axis value
    function M.getY()
        local val = getRawY();
        local ret = val * 390.0 / gainValue;

        return ret;
    end

    --get z-axis value
    function M.getZ()
        local val = getRawZ();
        local ret = val * 390.0 / gainValue;

        return ret;
    end

    -- get temperature
    function M.getTemperature()
        local msb = read_reg(0x31);
        local lsb = read_reg(0x32);

        local temp = msb * 256 + lsb;

        return temp;

    end

