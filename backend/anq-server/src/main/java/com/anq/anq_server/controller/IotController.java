package com.anq.anq_server.controller;

import com.anq.anq_server.entity.DeviceData;
import com.anq.anq_server.repository.DeviceDataRepository;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.*;

import java.util.Map;

@RestController
@RequestMapping("/iot")
public class IotController {

    @Autowired
    private DeviceDataRepository repository;

    @PostMapping("/callback")
    public String receive(@RequestBody Map<String, Object> payload) {

        DeviceData data = new DeviceData();
        data.setDeviceId((String) payload.get("device_id"));
        data.setTemperature(Double.valueOf(payload.get("temperature").toString()));
        data.setHumidity(Double.valueOf(payload.get("humidity").toString()));
        data.setTimestamp(Long.valueOf(payload.get("timestamp").toString()));

        repository.save(data);

        return "ok";
    }
}