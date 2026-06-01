package com.anq.anq_server.controller;

import com.anq.anq_server.entity.Alert;
import com.anq.anq_server.entity.DeviceData;
import com.anq.anq_server.repository.DeviceDataRepository;
import com.anq.anq_server.service.AlertService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.*;

import java.time.Instant;
import java.time.LocalDateTime;
import java.time.ZoneId;
import java.time.format.DateTimeFormatter;
import java.util.List;
import java.util.Map;

@RestController
@RequestMapping("/iot")
public class IotController {

    @Autowired
    private DeviceDataRepository repository;

    @Autowired
    private AlertService alertService;

    @PostMapping("/callback")
    public String receive(@RequestBody Map<String, Object> payload) {
        Map<String, Object> properties = extractProperties(payload);
        if (properties != null && properties.containsKey("alarmType")) {
            Alert alert = new Alert();
            alert.setAlarmType(asString(properties.get("alarmType")));
            alert.setConfidence(asDouble(properties.get("confidence")));
            alert.setLocation(firstNonBlank(asString(properties.get("location")), "未知位置"));
            alert.setAudioLevel(asInteger(properties.get("audioLevel")));
            alert.setRadarData(asString(properties.get("radarData")));
            alert.setEvidenceUrl(asString(properties.get("evidenceUrl")));
            alert.setTime(formatTimestamp(asLong(properties.get("timestamp"))));
            alert.setStatus("NEW");
            alertService.saveAndPush(alert);
            return "ok";
        }

        DeviceData data = new DeviceData();
        data.setDeviceId(asString(payload.get("device_id")));
        data.setTemperature(asDouble(payload.get("temperature")));
        data.setHumidity(asDouble(payload.get("humidity")));
        data.setTimestamp(asLong(payload.get("timestamp")));

        repository.save(data);
        return "ok";
    }

    @SuppressWarnings("unchecked")
    private Map<String, Object> extractProperties(Map<String, Object> payload) {
        Object servicesObj = payload.get("services");
        if (!(servicesObj instanceof List<?> services) || services.isEmpty()) {
            return null;
        }
        Object serviceObj = services.get(0);
        if (!(serviceObj instanceof Map<?, ?> service)) {
            return null;
        }
        Object propertiesObj = service.get("properties");
        if (propertiesObj instanceof Map<?, ?> properties) {
            return (Map<String, Object>) properties;
        }
        return null;
    }

    private String firstNonBlank(String... values) {
        for (String value : values) {
            if (value != null && !value.isBlank()) {
                return value;
            }
        }
        return null;
    }

    private String asString(Object value) {
        return value == null ? null : value.toString();
    }

    private Double asDouble(Object value) {
        return value == null ? null : Double.valueOf(value.toString());
    }

    private Integer asInteger(Object value) {
        return value == null ? null : Integer.valueOf(value.toString());
    }

    private Long asLong(Object value) {
        return value == null ? null : Long.valueOf(value.toString());
    }

    private String formatTimestamp(Long timestamp) {
        if (timestamp == null) {
            return null;
        }
        return LocalDateTime.ofInstant(Instant.ofEpochMilli(timestamp), ZoneId.systemDefault())
                .format(DateTimeFormatter.ofPattern("yyyy-MM-dd HH:mm:ss"));
    }
}