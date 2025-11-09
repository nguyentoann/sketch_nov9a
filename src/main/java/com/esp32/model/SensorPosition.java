package com.esp32.model;

public class SensorPosition {
    private String sensorId;  // e.g., "mq1", "mq2", "mq3", "temperature", "humidity", "dust"
    private String roomName;   // e.g., "Living Room", "Bedroom", "Kitchen"
    private double x;          // X position on image (percentage or pixels)
    private double y;          // Y position on image (percentage or pixels)
    
    public SensorPosition() {
    }
    
    public SensorPosition(String sensorId, String roomName, double x, double y) {
        this.sensorId = sensorId;
        this.roomName = roomName;
        this.x = x;
        this.y = y;
    }
    
    // Getters and Setters
    public String getSensorId() {
        return sensorId;
    }
    
    public void setSensorId(String sensorId) {
        this.sensorId = sensorId;
    }
    
    public String getRoomName() {
        return roomName;
    }
    
    public void setRoomName(String roomName) {
        this.roomName = roomName;
    }
    
    public double getX() {
        return x;
    }
    
    public void setX(double x) {
        this.x = x;
    }
    
    public double getY() {
        return y;
    }
    
    public void setY(double y) {
        this.y = y;
    }
}

