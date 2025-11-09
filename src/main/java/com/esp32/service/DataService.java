package com.esp32.service;

import com.esp32.model.SensorData;
import com.esp32.model.SensorPosition;
import com.esp32.model.Thresholds;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.atomic.AtomicReference;
import java.util.Map;

public class DataService {
    private static final DataService instance = new DataService();
    
    // Store latest sensor data
    private final AtomicReference<SensorData> latestData = new AtomicReference<>(new SensorData());
    
    // Store thresholds
    private final AtomicReference<Thresholds> thresholds = new AtomicReference<>(new Thresholds());
    
    // Store sensor positions on house map
    private final Map<String, SensorPosition> sensorPositions = new ConcurrentHashMap<>();
    
    // Track if thresholds have been updated (for polling)
    private volatile boolean thresholdsUpdated = false;
    
    private DataService() {
    }
    
    public static DataService getInstance() {
        return instance;
    }
    
    public void saveSensorData(SensorData data) {
        latestData.set(data);
    }
    
    public SensorData getLatestData() {
        return latestData.get();
    }
    
    public Thresholds getThresholds() {
        return thresholds.get();
    }
    
    public void updateThresholds(Thresholds newThresholds) {
        thresholds.set(newThresholds);
        thresholdsUpdated = true;
    }
    
    public boolean hasThresholdsUpdate() {
        boolean updated = thresholdsUpdated;
        thresholdsUpdated = false; // Reset after checking
        return updated;
    }
    
    public boolean checkThresholds(SensorData data) {
        Thresholds th = thresholds.get();
        
        return data.getMq1() >= th.getMq1() ||
               data.getMq2() >= th.getMq2() ||
               data.getMq3() >= th.getMq3() ||
               data.getTemperature() >= th.getTemperature() ||
               data.getDust() >= th.getDust();
    }
    
    public String getAlertMessage(SensorData data) {
        Thresholds th = thresholds.get();
        StringBuilder alerts = new StringBuilder();
        
        if (data.getMq1() >= th.getMq1()) {
            alerts.append("MQ1 threshold exceeded (").append(data.getMq1()).append(" >= ").append(th.getMq1()).append("). ");
        }
        if (data.getMq2() >= th.getMq2()) {
            alerts.append("MQ2 threshold exceeded (").append(data.getMq2()).append(" >= ").append(th.getMq2()).append("). ");
        }
        if (data.getMq3() >= th.getMq3()) {
            alerts.append("MQ3 threshold exceeded (").append(data.getMq3()).append(" >= ").append(th.getMq3()).append("). ");
        }
        if (data.getTemperature() >= th.getTemperature()) {
            alerts.append("Temperature threshold exceeded (").append(data.getTemperature()).append(" >= ").append(th.getTemperature()).append("). ");
        }
        if (data.getDust() >= th.getDust()) {
            alerts.append("Dust threshold exceeded (").append(data.getDust()).append(" >= ").append(th.getDust()).append("). ");
        }
        
        return alerts.length() > 0 ? alerts.toString() : null;
    }
    
    // Sensor position methods
    public void saveSensorPosition(SensorPosition position) {
        sensorPositions.put(position.getSensorId(), position);
    }
    
    public SensorPosition getSensorPosition(String sensorId) {
        return sensorPositions.get(sensorId);
    }
    
    public Map<String, SensorPosition> getAllSensorPositions() {
        return new ConcurrentHashMap<>(sensorPositions);
    }
    
    public void saveAllSensorPositions(Map<String, SensorPosition> positions) {
        sensorPositions.clear();
        sensorPositions.putAll(positions);
    }
    
    public void deleteSensorPosition(String sensorId) {
        sensorPositions.remove(sensorId);
    }
}

