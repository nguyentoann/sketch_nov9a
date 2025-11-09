package com.esp32.servlet;

import com.esp32.model.SensorData;
import com.esp32.service.DataService;
import com.google.gson.Gson;
import javax.servlet.ServletException;
import javax.servlet.annotation.WebServlet;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import java.io.BufferedReader;
import java.io.IOException;

@WebServlet("/api/data")
public class DataServlet extends HttpServlet {
    private final Gson gson = new Gson();
    private final DataService dataService = DataService.getInstance();
    
    @Override
    protected void doPost(HttpServletRequest request, HttpServletResponse response)
            throws ServletException, IOException {
        response.setContentType("application/json");
        response.setCharacterEncoding("UTF-8");
        response.setHeader("Access-Control-Allow-Origin", "*");
        response.setHeader("Access-Control-Allow-Methods", "POST, OPTIONS");
        response.setHeader("Access-Control-Allow-Headers", "Content-Type");
        
        if ("OPTIONS".equalsIgnoreCase(request.getMethod())) {
            response.setStatus(HttpServletResponse.SC_OK);
            return;
        }
        
        try {
            // Read JSON from request body
            StringBuilder json = new StringBuilder();
            String line;
            try (BufferedReader reader = request.getReader()) {
                while ((line = reader.readLine()) != null) {
                    json.append(line);
                }
            }
            
            // Parse JSON to SensorData
            SensorData sensorData = gson.fromJson(json.toString(), SensorData.class);
            
            // Save to service
            dataService.saveSensorData(sensorData);
            
            // Return success response
            response.setStatus(HttpServletResponse.SC_OK);
            response.getWriter().write("{\"status\":\"success\",\"message\":\"Data received\"}");
            
        } catch (Exception e) {
            response.setStatus(HttpServletResponse.SC_BAD_REQUEST);
            response.getWriter().write("{\"status\":\"error\",\"message\":\"" + e.getMessage() + "\"}");
        }
    }
}

