package com.anq.anq_server.controller;

import com.anq.anq_server.entity.Alert;
import com.anq.anq_server.service.AlertService;
import org.springframework.web.bind.annotation.*;

import java.time.Instant;
import java.time.LocalDateTime;
import java.time.ZoneId;
import java.time.format.DateTimeFormatter;
import java.util.List;

@RestController
@RequestMapping("/alert")
@CrossOrigin(origins = "*")
public class AlertController {

    private final AlertService alertService;

    public AlertController(AlertService alertService) {
        this.alertService = alertService;
    }

    @GetMapping("/create")
    public String createAlert(@RequestParam(defaultValue = "未知位置") String location) {
        alertService.createAndPushAlert(location, "NEW");
        return "✅ 警情已创建并推送！位置: " + location;
    }

    @PostMapping("/create")
    public String createAlertJson(@RequestBody(required = false) AlertPayload dto) {
        Alert alert = toAlert(dto);
        alertService.saveAndPush(alert);
        return "✅ 警情已创建并推送！位置: " + alert.getLocation();
    }

    @PostMapping({"", "/"})
    public String receiveAlert(@RequestBody AlertPayload dto) {
        alertService.saveAndPush(toAlert(dto));
        return "ok";
    }

    @GetMapping("/all")
    public List<Alert> getAllAlerts() {
        return alertService.getRecentAlerts();
    }

    private Alert toAlert(AlertPayload dto) {
        AlertPayload.PropertiesDto properties = extractProperties(dto);
        Alert alert = new Alert();
        alert.setLocation(firstNonBlank(
                properties == null ? null : properties.location,
                dto == null ? null : dto.location,
                "未知位置"));
        alert.setTime(firstNonBlank(
                dto == null ? null : dto.time,
                formatTimestamp(properties == null ? null : properties.timestamp),
                null));
        alert.setStatus(firstNonBlank(dto == null ? null : dto.status, "NEW"));
        if (properties != null) {
            alert.setAlarmType(properties.alarmType);
            alert.setConfidence(properties.confidence);
            alert.setAudioLevel(properties.audioLevel);
            alert.setRadarData(properties.radarData);
            alert.setEvidenceUrl(properties.evidenceUrl);
        }
        return alert;
    }

    private AlertPayload.PropertiesDto extractProperties(AlertPayload dto) {
        if (dto == null || dto.services == null || dto.services.isEmpty()) {
            return null;
        }
        AlertPayload.ServiceDto service = dto.services.get(0);
        return service == null ? null : service.properties;
    }

    private String firstNonBlank(String... values) {
        for (String value : values) {
            if (value != null && !value.isBlank()) {
                return value;
            }
        }
        return null;
    }

    private String formatTimestamp(Long timestamp) {
        if (timestamp == null) {
            return null;
        }
        return LocalDateTime.ofInstant(Instant.ofEpochMilli(timestamp), ZoneId.systemDefault())
                .format(DateTimeFormatter.ofPattern("yyyy-MM-dd HH:mm:ss"));
    }

    public static class AlertPayload {
        private String location;
        private String time;
        private String status;
        private List<ServiceDto> services;

        public String getLocation() { return location; }
        public void setLocation(String location) { this.location = location; }
        public String getTime() { return time; }
        public void setTime(String time) { this.time = time; }
        public String getStatus() { return status; }
        public void setStatus(String status) { this.status = status; }
        public List<ServiceDto> getServices() { return services; }
        public void setServices(List<ServiceDto> services) { this.services = services; }

        public static class ServiceDto {
            private PropertiesDto properties;
            public PropertiesDto getProperties() { return properties; }
            public void setProperties(PropertiesDto properties) { this.properties = properties; }
        }

        public static class PropertiesDto {
            private String alarmType;
            private Double confidence;
            private Long timestamp;
            private String location;
            private Integer audioLevel;
            private String radarData;
            private String evidenceUrl;

            public String getAlarmType() { return alarmType; }
            public void setAlarmType(String alarmType) { this.alarmType = alarmType; }
            public Double getConfidence() { return confidence; }
            public void setConfidence(Double confidence) { this.confidence = confidence; }
            public Long getTimestamp() { return timestamp; }
            public void setTimestamp(Long timestamp) { this.timestamp = timestamp; }
            public String getLocation() { return location; }
            public void setLocation(String location) { this.location = location; }
            public Integer getAudioLevel() { return audioLevel; }
            public void setAudioLevel(Integer audioLevel) { this.audioLevel = audioLevel; }
            public String getRadarData() { return radarData; }
            public void setRadarData(String radarData) { this.radarData = radarData; }
            public String getEvidenceUrl() { return evidenceUrl; }
            public void setEvidenceUrl(String evidenceUrl) { this.evidenceUrl = evidenceUrl; }
        }
    }
}
