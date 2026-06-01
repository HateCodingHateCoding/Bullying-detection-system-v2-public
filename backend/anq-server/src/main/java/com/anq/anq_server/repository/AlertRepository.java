package com.anq.anq_server.repository;

import com.anq.anq_server.entity.Alert;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.data.jpa.repository.Query;
import org.springframework.data.repository.query.Param;
import org.springframework.stereotype.Repository;

import java.time.LocalDateTime;
import java.util.List;

/**
 * Alert 数据访问层
 * 继承 JpaRepository<实体类, ID类型> 后，自动拥有 save, findById, delete 等方法
 */
@Repository
public interface AlertRepository extends JpaRepository<Alert, Long> {

    /**
     * 自定义查询方法：按创建时间倒序，获取最新的 10 条告警
     */
    List<Alert> findTop10ByOrderByCreateTimeDesc();

    /**
     * 统计指定时间范围内的告警数量
     */
    long countByCreateTimeBetween(LocalDateTime start, LocalDateTime end);

    /**
     * 统计处理中的告警数量
     */
    long countByStatus(String status);

    /**
     * 获取指定时间范围内的所有告警（用于分时段/分区域统计）
     */
    List<Alert> findByCreateTimeAfter(LocalDateTime since);
}