package com.esp32.model;

public class Thresholds {
    private int mq1;
    private int mq2;
    private int mq3;
    private float temperature;
    private float dust;
    
    public Thresholds() {
        // Default thresholds
        this.mq1 = 300;
        this.mq2 = 300;
        this.mq3 = 600;
        this.temperature = 30.0f;
        this.dust = 50.0f;
    }
    
    public Thresholds(int mq1, int mq2, int mq3, float temperature, float dust) {
        this.mq1 = mq1;
        this.mq2 = mq2;
        this.mq3 = mq3;
        this.temperature = temperature;
        this.dust = dust;
    }
    
    // Getters and Setters
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
    
    public float getTemperature() {
        return temperature;
    }
    
    public void setTemperature(float temperature) {
        this.temperature = temperature;
    }
    
    public float getDust() {
        return dust;
    }
    
    public void setDust(float dust) {
        this.dust = dust;
    }
}

