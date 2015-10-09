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

    local deviceAddress = 0x1e
    local gainValue = 0;

    -- read reg for 1 byte
    local function read_reg(reg_addr)
      i2c.start(id)
      i2c.address(id, deviceAddress, i2c.TRANSMITTER)
      i2c.write(id, reg_addr)
      i2c.stop(id)
      i2c.start(id)
      i2c.address(id, deviceAddress, i2c.RECEIVER)
      local c = i2c.read(id, 1)

      --  print("read: ");
      --  print(string.byte(c));
      
      i2c.stop(id)
      return c
    end   

    --write reg for 1 byte
    local function write_reg(reg_addr, reg_val)
      i2c.start(id)
      i2c.address(id, deviceAddress, i2c.TRANSMITTER)
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

        M.enableMagnetometer();
        M.setMagneticGain(1);

    end

    -- enable the magnetometer
    function M.enableMagnetometer()
        write_reg(0x02, 0x00); -- mode register
    end

    function M.dump()
        read_reg(0x00);
        read_reg(0x01);
        read_reg(0x02);
        read_reg(0x03);
        read_reg(0x04);
        read_reg(0x05);
        read_reg(0x06);
        read_reg(0x07);
        read_reg(0x08);
        read_reg(0x09);
        read_reg(0x0A);
        read_reg(0x0B);
        read_reg(0x0C);
    end


    --set the magnetic gain
    --parameters:
    --num: 0 to 7
    function M.setMagneticGain(gain)
      if (gain >=0) and (gain <= 7) then
        write_reg(0x01, gain);

        if (gain == 0) then
            gainValue = 73;
        end
        
        if (gain == 1) then
            gainValue = 92;
        end
        
        if (gain == 2) then
            gainValue = 122;
        end
        
        if (gain == 3) then
            gainValue = 152;
        end
        
        if (gain == 4) then
            gainValue = 227;
        end
        
        if (gain == 5) then
            gainValue = 256;
        end
        
        if (gain == 6) then
            gainValue = 303;
        end
        
        if (gain == 7) then
            gainValue = 435;
        end
      end
    end

    --get x-axis value
    function M.getX()
        local val = getShort(0x03, true);
        local ret = val * gainValue / 100;

        return ret;
    end

    --get y-axis value
    function M.getY()
        local val = getShort(0x07, true);
        local ret = val * gainValue / 100;

        return ret;
    end

    --get z-axis value
    function M.getZ()
        local val = getShort(0x05, true);
        local ret = val * gainValue / 100;

        return ret;
    end


    return M
