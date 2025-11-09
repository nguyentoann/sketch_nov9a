package com.esp32.servlet;

import javax.servlet.ServletException;
import javax.servlet.annotation.WebServlet;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import java.io.IOException;
import java.io.InputStream;

@WebServlet("/sensor-data.html")
public class SensorDataServlet extends HttpServlet {
    
    @Override
    protected void doGet(HttpServletRequest request, HttpServletResponse response)
            throws ServletException, IOException {
        response.setContentType("text/html");
        response.setCharacterEncoding("UTF-8");
        
        // Read sensor-data.html from resources
        try (InputStream is = getServletContext().getResourceAsStream("/WEB-INF/sensor-data.html")) {
            if (is != null) {
                byte[] buffer = new byte[8192];
                int bytesRead;
                while ((bytesRead = is.read(buffer)) != -1) {
                    response.getOutputStream().write(buffer, 0, bytesRead);
                }
                response.getOutputStream().flush();
            } else {
                response.setStatus(HttpServletResponse.SC_NOT_FOUND);
                response.getWriter().write("<h1>Sensor Data page not found</h1>");
            }
        } catch (IOException e) {
            response.setStatus(HttpServletResponse.SC_INTERNAL_SERVER_ERROR);
            response.getWriter().write("<h1>Error loading sensor data page: " + e.getMessage() + "</h1>");
        }
    }
}

