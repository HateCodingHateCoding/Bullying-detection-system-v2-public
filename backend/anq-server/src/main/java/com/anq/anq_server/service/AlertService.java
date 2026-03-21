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
    public void createAndPushAlert(String location, String status) {
        // 1. 构建 Alert 对象
        Alert alert = new Alert();
        alert.setLocation(location);
        alert.setStatus(status);
        alert.setTime(LocalDateTime.now().format(DateTimeFormatter.ofPattern("yyyy-MM-dd HH:mm:ss")));

        // 2. 存入数据库
        Alert savedAlert = alertRepository.save(alert);
        System.out.println("💾 告警已入库，ID: " + savedAlert.getId());

        // 3. 通过 WebSocket 实时推送给所有在线 App
        webSocketHandler.sendAlert(savedAlert);
        System.out.println("📢 告警已推送至前端");
    }

    /**
     * 获取历史告警列表 (给 App 初始化或刷新用)
     */
    public List<Alert> getRecentAlerts() {
        return alertRepository.findTop10ByOrderByCreateTimeDesc();
    }
}