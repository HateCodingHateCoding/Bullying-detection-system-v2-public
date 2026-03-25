package com.bullying.alert.controller;

import com.bullying.alert.dto.IoTDAMessage;
import com.bullying.alert.entity.Alert;
import org.springframework.web.bind.annotation.*;
import java.time.Instant;
import java.time.LocalDateTime;
import java.time.ZoneId;
import java.time.format.DateTimeFormatter;
import java.util.concurrent.atomic.AtomicLong;

@RestController
@CrossOrigin(origins = "*")
public class IoTDAController {

    private final AtomicLong idGenerator = new AtomicLong(1);
    private Alert latestAlert = null;
    private final DateTimeFormatter formatter = DateTimeFormatter.ofPattern("yyyy-MM-dd HH:mm:ss");

    @PostMapping("/iotda/callback")
    public void receiveFromIoTDA(@RequestBody IoTDAMessage message) {
        if (message.getServices() != null && message.getServices().length > 0) {
            IoTDAMessage.Properties props = message.getServices()[0].getProperties();

            Alert alert = new Alert();
            alert.setId(idGenerator.getAndIncrement());
            alert.setLocation(props.getLocation());
            alert.setTime(LocalDateTime.ofInstant(
                Instant.ofEpochMilli(props.getTimestamp()),
                ZoneId.systemDefault()
            ).format(formatter));
            alert.setStatus("NEW");
            alert.setConfidence(props.getConfidence());
            alert.setEvidenceUrl(props.getEvidenceUrl());
            alert.setCreateTime(LocalDateTime.now());

            latestAlert = alert;

            System.out.println("📥 IoTDA推送: " + alert.getLocation() +
                ", 置信度: " + String.format("%.1f%%", alert.getConfidence() * 100) +
                ", 证据: " + alert.getEvidenceUrl());
        }
    }

    @GetMapping("/alert/latest")
    public Alert getLatestAlert() {
        return latestAlert;
    }
}
