package com.anq.anq_server.repository;

import com.anq.anq_server.entity.Alert;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;
import java.util.List;

/**
 * Alert 数据访问层
 * 继承 JpaRepository<实体类, ID类型> 后，自动拥有 save, findById, delete 等方法
 */
@Repository
public interface AlertRepository extends JpaRepository<Alert, Long> {

    /**
     * 自定义查询方法：按创建时间倒序，获取最新的 10 条告警
     * 方法命名规则：find + Top + 数量 + By + 排序字段 + OrderBy + 方向
     */
    List<Alert> findTop10ByOrderByCreateTimeDesc();
}