package com.esp32.server;

import org.eclipse.jetty.server.Server;
import org.eclipse.jetty.servlet.ServletContextHandler;
import org.eclipse.jetty.servlet.ServletHolder;
import org.eclipse.jetty.servlet.FilterHolder;
import com.esp32.servlet.DataServlet;
import com.esp32.servlet.PollServlet;
import com.esp32.servlet.ThresholdServlet;
import com.esp32.servlet.LatestDataServlet;
import com.esp32.servlet.DashboardServlet;
import com.esp32.servlet.SensorDataServlet;
import com.esp32.servlet.SensorPositionServlet;
import com.esp32.filter.CORSFilter;
import java.io.File;

public class EmbeddedServer {
    private static final int DEFAULT_PORT = 8085;
    
    public static void main(String[] args) {
        int port = DEFAULT_PORT;
        if (args.length > 0) {
            try {
                port = Integer.parseInt(args[0]);
            } catch (NumberFormatException e) {
                System.err.println("Invalid port number: " + args[0]);
                System.err.println("Using default port: " + DEFAULT_PORT);
            }
        }
        
        Server server = new Server(port);
        ServletContextHandler context = new ServletContextHandler(ServletContextHandler.SESSIONS);
        context.setContextPath("/");
        
        // Set resource base to webapp directory
        String webappDir = getWebappDirectory();
        context.setResourceBase(webappDir);
        context.setInitParameter("org.eclipse.jetty.servlet.Default.dirAllowed", "false");
        
        // Register CORS filter first
        FilterHolder corsFilter = new FilterHolder(CORSFilter.class);
        context.addFilter(corsFilter, "/*", null);
        
        // Register servlets
        context.addServlet(new ServletHolder(new DashboardServlet()), "/");
        context.addServlet(new ServletHolder(new SensorDataServlet()), "/sensor-data.html");
        context.addServlet(new ServletHolder(new DataServlet()), "/api/data");
        context.addServlet(new ServletHolder(new PollServlet()), "/api/poll");
        context.addServlet(new ServletHolder(new ThresholdServlet()), "/api/thresholds");
        context.addServlet(new ServletHolder(new LatestDataServlet()), "/api/latest");
        context.addServlet(new ServletHolder(new SensorPositionServlet()), "/api/sensor-positions");
        
        server.setHandler(context);
        
        try {
            server.start();
            System.out.println("========================================");
            System.out.println("ESP32 Sensor Server Started!");
            System.out.println("========================================");
            System.out.println("Server running on: http://localhost:" + port);
            System.out.println("Dashboard: http://localhost:" + port + "/");
            System.out.println("API Base: http://localhost:" + port + "/api/");
            System.out.println("Webapp directory: " + webappDir);
            System.out.println("========================================");
            System.out.println("Press Ctrl+C to stop the server");
            System.out.println("========================================");
            server.join();
        } catch (Exception e) {
            System.err.println("Error starting server: " + e.getMessage());
            e.printStackTrace();
            System.exit(1);
        }
    }
    
    private static String getWebappDirectory() {
        // Try to find webapp directory relative to current working directory
        File webappDir = new File("src/main/webapp");
        if (webappDir.exists() && webappDir.isDirectory()) {
            return webappDir.getAbsolutePath();
        }
        
        // Try alternative path (if running from target directory)
        webappDir = new File("../src/main/webapp");
        if (webappDir.exists() && webappDir.isDirectory()) {
            return webappDir.getAbsolutePath();
        }
        
        // Default fallback
        return "src/main/webapp";
    }
}

