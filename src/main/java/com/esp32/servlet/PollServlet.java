package com.esp32.servlet;

import com.esp32.model.Thresholds;
import com.esp32.service.DataService;
import com.google.gson.Gson;
import javax.servlet.ServletException;
import javax.servlet.annotation.WebServlet;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import java.io.IOException;

@WebServlet("/api/poll")
public class PollServlet extends HttpServlet {
    private final Gson gson = new Gson();
    private final DataService dataService = DataService.getInstance();
    
    @Override
    protected void doGet(HttpServletRequest request, HttpServletResponse response)
            throws ServletException, IOException {
        response.setContentType("application/json");
        response.setCharacterEncoding("UTF-8");
        response.setHeader("Access-Control-Allow-Origin", "*");
        response.setHeader("Access-Control-Allow-Methods", "GET, OPTIONS");
        response.setHeader("Access-Control-Allow-Headers", "Content-Type");
        
        if ("OPTIONS".equalsIgnoreCase(request.getMethod())) {
            response.setStatus(HttpServletResponse.SC_OK);
            return;
        }
        
        try {
            // Check if thresholds have been updated
            if (dataService.hasThresholdsUpdate()) {
                Thresholds thresholds = dataService.getThresholds();
                String json = gson.toJson(thresholds);
                response.setStatus(HttpServletResponse.SC_OK);
                response.getWriter().write("{\"update\":true,\"thresholds\":" + json + "}");
            } else {
                // No update
                response.setStatus(HttpServletResponse.SC_OK);
                response.getWriter().write("{\"update\":false}");
            }
        } catch (Exception e) {
            response.setStatus(HttpServletResponse.SC_INTERNAL_SERVER_ERROR);
            response.getWriter().write("{\"status\":\"error\",\"message\":\"" + e.getMessage() + "\"}");
        }
    }
}

