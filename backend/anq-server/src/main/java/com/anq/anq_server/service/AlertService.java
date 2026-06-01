package com.anq.anq_server.service;

import com.anq.anq_server.entity.Alert;
import com.anq.anq_server.repository.AlertRepository;
import com.anq.anq_server.ws.AlertWebSocketHandler; // 确保类名正确
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import java.time.LocalDateTime;
import java.time.format.DateTimeFormatter;
import java.util.List;

@Service
public class AlertService {

    @Autowired
    private AlertRepository alertRepository;

    @Autowired
    private AlertWebSocketHandler webSocketHandler; // 注入 WebSocket 处理器

    /**
     * 创建告警并推送
     */
    @Transactional
    public Alert createAndPushAlert(String location, String status) {
        Alert alert = new Alert();
        alert.setLocation(location);
        alert.setStatus(status);
        alert.setTime(LocalDateTime.now().format(DateTimeFormatter.ofPattern("yyyy-MM-dd HH:mm:ss")));
        return saveAndPush(alert);
    }

    @Transactional
    public Alert saveAndPush(Alert alert) {
        if (alert.getStatus() == null) {
            alert.setStatus("NEW");
        }
        if (alert.getTime() == null) {
            alert.setTime(LocalDateTime.now().format(DateTimeFormatter.ofPattern("yyyy-MM-dd HH:mm:ss")));
        }

        Alert savedAlert = alertRepository.save(alert);
        System.out.println("💾 告警已入库，ID: " + savedAlert.getId());

        webSocketHandler.sendAlert(savedAlert);
        System.out.println("📢 告警已推送至前端");
        return savedAlert;
    }

    /**
     * 获取历史告警列表 (给 App 初始化或刷新用)
     */
    public List<Alert> getRecentAlerts() {
        return alertRepository.findTop10ByOrderByCreateTimeDesc();
    }
}