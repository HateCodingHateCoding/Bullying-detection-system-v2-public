package com.bullying.alert.controller;

import com.bullying.alert.entity.Alert;
import org.springframework.web.bind.annotation.*;
import java.time.LocalDateTime;
import java.util.concurrent.atomic.AtomicLong;

@RestController
@RequestMapping("/alert")
@CrossOrigin(origins = "*")
public class AlertController {

    private final AtomicLong idGenerator = new AtomicLong(1);
    private Alert latestAlert = null;

    @PostMapping
    public Alert createAlert(@RequestBody Alert alert) {
        alert.setId(idGenerator.getAndIncrement());
        alert.setCreateTime(LocalDateTime.now());
        latestAlert = alert;

        System.out.println("📥 收到告警: " + alert.getLocation() +
            (alert.getConfidence() != null ? ", 置信度: " + String.format("%.1f%%", alert.getConfidence() * 100) : "") +
            (alert.getEvidenceUrl() != null ? ", 证据: " + alert.getEvidenceUrl() : ""));

        return alert;
    }

    @GetMapping("/latest")
    public Alert getLatestAlert() {
        return latestAlert;
    }
}
