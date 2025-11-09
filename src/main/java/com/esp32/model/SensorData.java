package com.esp32.model;

import java.time.LocalDateTime;
import java.time.format.DateTimeFormatter;

public class SensorData {
    private float temperature;
    private float humidity;
    private int mq1;
    private int mq2;
    private int mq3;
    private float dust;
    private int wifiSignal;
    private long uptime; // in seconds
    private String timestamp;
    
    public SensorData() {
        this.timestamp = LocalDateTime.now().format(DateTimeFormatter.ISO_LOCAL_DATE_TIME);
    }
    
    // Getters and Setters
    public float getTemperature() {
        return temperature;
    }
    
    public void setTemperature(float temperature) {
        this.temperature = temperature;
    }
    
    public float getHumidity() {
        return humidity;
    }
    
    public void setHumidity(float humidity) {
        this.humidity = humidity;
    }
    
    public int getMq1() {
        return mq1;
    }
    
    public void setMq1(int mq1) {
        this.mq1 = mq1;
    }
    
    public int getMq2() {
        return mq2;
    }
    
    public void setMq2(int mq2) {
        this.mq2 = mq2;
    }
    
    public int getMq3() {
        return mq3;
    }
    
    public void setMq3(int mq3) {
        this.mq3 = mq3;
    }
    
    public float getDust() {
        return dust;
    }
    
    public void setDust(float dust) {
        this.dust = dust;
    }
    
    public int getWifiSignal() {
        return wifiSignal;
    }
    
    public void setWifiSignal(int wifiSignal) {
        this.wifiSignal = wifiSignal;
    }
    
    public long getUptime() {
        return uptime;
    }
    
    public void setUptime(long uptime) {
        this.uptime = uptime;
    }
    
    public String getTimestamp() {
        return timestamp;
    }
    
    public void setTimestamp(String timestamp) {
        this.timestamp = timestamp;
    }
}

