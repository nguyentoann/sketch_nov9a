package com.esp32.servlet;

import com.esp32.model.SensorPosition;
import com.esp32.service.DataService;
import com.google.gson.Gson;
import com.google.gson.reflect.TypeToken;
import javax.servlet.ServletException;
import javax.servlet.annotation.WebServlet;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import java.io.BufferedReader;
import java.io.IOException;
import java.lang.reflect.Type;
import java.util.Map;

@WebServlet("/api/sensor-positions")
public class SensorPositionServlet extends HttpServlet {
    private final Gson gson = new Gson();
    private final DataService dataService = DataService.getInstance();
    
    @Override
    protected void doGet(HttpServletRequest request, HttpServletResponse response)
            throws ServletException, IOException {
        response.setContentType("application/json");
        response.setCharacterEncoding("UTF-8");
        response.setHeader("Access-Control-Allow-Origin", "*");
        response.setHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        response.setHeader("Access-Control-Allow-Headers", "Content-Type");
        
        if ("OPTIONS".equalsIgnoreCase(request.getMethod())) {
            response.setStatus(HttpServletResponse.SC_OK);
            return;
        }
        
        try {
            Map<String, SensorPosition> positions = dataService.getAllSensorPositions();
            String json = gson.toJson(positions);
            response.setStatus(HttpServletResponse.SC_OK);
            response.getWriter().write(json);
        } catch (Exception e) {
            response.setStatus(HttpServletResponse.SC_INTERNAL_SERVER_ERROR);
            response.getWriter().write("{\"status\":\"error\",\"message\":\"" + e.getMessage() + "\"}");
        }
    }
    
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
            
            // Check if it's a single position or map of positions
            String jsonStr = json.toString();
            
            if (jsonStr.trim().startsWith("{")) {
                // Try to parse as map first (for bulk save)
                try {
                    Type mapType = new TypeToken<Map<String, SensorPosition>>(){}.getType();
                    Map<String, SensorPosition> positions = gson.fromJson(jsonStr, mapType);
                    dataService.saveAllSensorPositions(positions);
                    response.setStatus(HttpServletResponse.SC_OK);
                    response.getWriter().write("{\"status\":\"success\",\"message\":\"Sensor positions updated\"}");
                    return;
                } catch (Exception e) {
                    // If map parsing fails, try single position
                }
                
                // Parse as single SensorPosition
                SensorPosition position = gson.fromJson(jsonStr, SensorPosition.class);
                dataService.saveSensorPosition(position);
                response.setStatus(HttpServletResponse.SC_OK);
                response.getWriter().write("{\"status\":\"success\",\"message\":\"Sensor position saved\"}");
            } else {
                response.setStatus(HttpServletResponse.SC_BAD_REQUEST);
                response.getWriter().write("{\"status\":\"error\",\"message\":\"Invalid JSON format\"}");
            }
            
        } catch (Exception e) {
            response.setStatus(HttpServletResponse.SC_BAD_REQUEST);
            response.getWriter().write("{\"status\":\"error\",\"message\":\"" + e.getMessage() + "\"}");
        }
    }
}

