package com.anq.anq_server.ws;

import com.anq.anq_server.entity.Alert;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.datatype.jsr310.JavaTimeModule; // 1. 导入时间模块
import com.fasterxml.jackson.databind.SerializationFeature; // 2. 导入序列化配置
import org.springframework.stereotype.Component;
import org.springframework.web.socket.TextMessage;
import org.springframework.web.socket.WebSocketSession;
import org.springframework.web.socket.handler.TextWebSocketHandler;

import java.util.ArrayList;
import java.util.List;

@Component
public class AlertWebSocketHandler extends TextWebSocketHandler {

    private final List<WebSocketSession> sessions = new ArrayList<>();

    // 3. 移除 final，因为在构造函数中需要初始化
    private final ObjectMapper objectMapper;

    // 4. 添加构造函数来配置 ObjectMapper
    public AlertWebSocketHandler() {
        this.objectMapper = new ObjectMapper();
        // 注册 JavaTimeModule 以支持 Java 8 时间类型 (LocalDateTime)
        this.objectMapper.registerModule(new JavaTimeModule());
        // 禁用将时间写为时间戳的功能，改为输出易读的字符串 (可选，但推荐)
        this.objectMapper.disable(SerializationFeature.WRITE_DATES_AS_TIMESTAMPS);
    }

    // ✅ 客户端连接时触发
    @Override
    public void afterConnectionEstablished(WebSocketSession session) {
        sessions.add(session);
        System.out.println("✅ WebSocket connected: " + session.getId());
        System.out.println("当前在线客户端数量：" + sessions.size());
    }

    // ✅ 客户端断开时触发
    @Override
    public void afterConnectionClosed(WebSocketSession session,
                                      org.springframework.web.socket.CloseStatus status) {
        sessions.remove(session);
        System.out.println("❌ WebSocket disconnected: " + session.getId());
        System.out.println("当前在线客户端数量：" + sessions.size());
    }

    // ✅ 推送警情
    public void sendAlert(Alert alert) {
        try {
            String json = objectMapper.writeValueAsString(alert);

            System.out.println("📢 准备推送警情");
            System.out.println("JSON 内容：" + json); // 打印生成的 JSON 方便调试
            System.out.println("当前在线客户端数量：" + sessions.size());

            for (WebSocketSession session : sessions) {
                if (session.isOpen()) {
                    session.sendMessage(new TextMessage(json));
                }
            }

            System.out.println("✅ 推送完成");

        } catch (Exception e) {
            System.err.println("❌ 推送失败：");
            e.printStackTrace();
        }
    }
}