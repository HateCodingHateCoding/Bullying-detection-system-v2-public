package com.anq.anq_server.controller;

import com.anq.anq_server.entity.Alert;
import com.anq.anq_server.service.AlertService;
import org.springframework.web.bind.annotation.*;

import java.time.LocalDateTime;
import java.time.format.DateTimeFormatter;
import java.util.List;

@RestController
@RequestMapping("/alert")
@CrossOrigin(origins = "*") // 允许跨域
public class AlertController {

    private final AlertService alertService;

    // 构造器注入
    public AlertController(AlertService alertService) {
        this.alertService = alertService;
    }

    /**
     * 1. 模拟创建警情 (测试用)
     * 访问示例: GET /alert/create?location=操场西侧
     * 或者 POST /alert/create with JSON {"location": "操场西侧"}
     */
    @GetMapping("/create")
    public String createAlert(@RequestParam(defaultValue = "未知位置") String location) {

        String timeStr = LocalDateTime.now().format(DateTimeFormatter.ofPattern("yyyy-MM-dd HH:mm:ss"));
        String status = "NEW"; // 默认状态

        // 调用 Service 层：自动存库 + 自动 WebSocket 推送
        // 注意：这里不再需要传 ID，ID 由数据库生成
        alertService.createAndPushAlert(location, status);

        return "✅ 警情已创建并推送！位置: " + location;
    }

    /**
     * 2. 接收 JSON 格式的创建请求 (更贴近真实硬件上报)
     * POST /alert/create (测试)
     * Body: { "location": "食堂门口" }
     */
    @PostMapping("/create")
    public String createAlertJson(@RequestBody(required = false) LocationDto dto) {
        String location = (dto != null && dto.getLocation() != null) ? dto.getLocation() : "未知位置";
        String timeStr = LocalDateTime.now().format(DateTimeFormatter.ofPattern("yyyy-MM-dd HH:mm:ss"));

        alertService.createAndPushAlert(location, "NEW");
        return "✅ 警情已创建并推送！位置: " + location;
    }
    // POST /alert/
    @PostMapping("")
    public String receiveAlert(@RequestBody LocationDto dto) {

        String location = "未知位置";
        // 1. 优先解析IoTDA的嵌套location（services[0].properties.location）
        if (dto != null && dto.getServices() != null && !dto.getServices().isEmpty()) {
            LocationDto.ServiceDto service = dto.getServices().get(0);
            if (service != null && service.getProperties() != null && service.getProperties().getLocation() != null) {
                location = service.getProperties().getLocation();
            }
        // 2. 兼容原有顶层location（保证旧参数格式仍可用）
        } else if (dto != null && dto.getLocation() != null) {
            location = dto.getLocation();
        }

        alertService.createAndPushAlert(location, "NEW");

        return "ok";
    }

    /**
     * 3. 获取所有历史警情 (给 App 初始化列表用)
     * GET /alert/all
     */
    @GetMapping("/all")
    public List<Alert> getAllAlerts() {
        return alertService.getRecentAlerts();
    }

    // 内部辅助类，用于接收 JSON
    public static class LocationDto {
        private String location;
        public String getLocation() { return location; }
        public void setLocation(String location) { this.location = location; }

        // 新增：解析IoTDA的services数组
        private List<ServiceDto> services;

        // 新增getter：获取services列表（必须加）
        public List<ServiceDto> getServices() {
            return this.services;
        }

        // 新增setter：设置services列表（可选，保证JSON解析）
        public void setServices(List<ServiceDto> services) {
            this.services = services;
        }

        // 新增内部类：解析service_id和properties
        public static class ServiceDto {
            private String service_id;
            private PropertiesDto properties;

            // 新增getter：获取properties
            public PropertiesDto getProperties() {
                return this.properties;
            }

            // 新增setter：设置properties（可选）
            public void setProperties(PropertiesDto properties) {
                this.properties = properties;
            }
        }

        // 新增内部类：解析location
        public static class PropertiesDto {
            private String location;

            // 新增getter：获取嵌套的location
            public String getLocation() {
                return this.location;
            }

            // 新增setter：设置嵌套的location（可选）
            public void setLocation(String location) {
                this.location = location;
            }
        }
    }
}