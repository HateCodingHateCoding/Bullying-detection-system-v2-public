package com.anq.anq_server.entity;

import jakarta.persistence.*;
import lombok.Data;
import java.time.LocalDateTime;

@Entity
@Table(name = "device_data")
@Data // Lombok 注解，自动生成 Getter/Setter/ToString
public class DeviceData {

    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    private Long id;

    @Column(name = "device_id", nullable = false, length = 50)
    private String deviceId;

    @Column(name = "temperature")
    private Double temperature;

    @Column(name = "humidity")
    private Double humidity;

    @Column(name = "event_timestamp")
    private Long timestamp; // 存储硬件传来的时间戳

    @Column(name = "create_time")
    private LocalDateTime createTime; // 记录存入数据库的时间

    // 在保存前自动设置创建时间
    @PrePersist
    protected void onCreate() {
        this.createTime = LocalDateTime.now();
    }
}