package com.anq.anq_server.entity;

import jakarta.persistence.*;
import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;
import java.time.LocalDateTime;

@Data
@AllArgsConstructor
@NoArgsConstructor // 必须有无参构造
@Entity
@Table(name = "alerts")
public class Alert {

    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY) // 数据库自增
    private Long id; // 必须是 Long

    private String location;

    private String time;

    private String status;

    @Column(updatable = false)
    private LocalDateTime createTime;

    @PrePersist
    protected void onCreate() {
        this.createTime = LocalDateTime.now();
        if (this.time == null) {
            this.time = this.createTime.toString();
        }
    }

    // 为了方便，提供一个不包含 ID 的构造函数（用于业务创建）
    public Alert(String location, String time, String status) {
        this.location = location;
        this.time = time;
        this.status = status;
    }
}