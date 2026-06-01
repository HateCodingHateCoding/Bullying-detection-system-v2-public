package com.anq.anq_server.controller;

import com.anq.anq_server.entity.Alert;
import com.anq.anq_server.repository.AlertRepository;
import org.springframework.web.bind.annotation.*;

import java.time.LocalDateTime;
import java.util.*;
import java.util.concurrent.ThreadLocalRandom;

/**
 * 演示数据生成接口 - 比赛展示前调用一次
 */
@RestController
@RequestMapping("/demo")
@CrossOrigin(origins = "*")
public class DemoController {

    private final AlertRepository alertRepository;

    public DemoController(AlertRepository alertRepository) {
        this.alertRepository = alertRepository;
    }

    /**
     * 清除旧演示数据并重新生成合理数量的模拟告警
     * 模拟场景：校园部署了4个监控点，日均3-6条告警
     */
    @GetMapping("/seed")
    public Map<String, Object> seedDemoData() {
        // 先清除旧的演示数据（保留真实告警）
        alertRepository.deleteAll();

        String[] locations = {"图书馆-1F-卫生间", "食堂后院", "3号楼楼梯间", "南操场入口"};
        String[] alarmTypes = {"bullying", "fighting", "verbal_abuse"};
        // 各时段概率不同
        int[][] timeSlots = {{7, 11}, {12, 13}, {14, 16}, {17, 19}, {20, 22}};

        List<Alert> generated = new ArrayList<>();
        LocalDateTime now = LocalDateTime.now();
        Random rng = ThreadLocalRandom.current();

        // 最近7天，每天3-6条
        for (int dayOffset = 0; dayOffset < 7; dayOffset++) {
            LocalDateTime day = now.minusDays(dayOffset);
            int dailyCount = 3 + rng.nextInt(4); // 3~6条/天
            for (int i = 0; i < dailyCount; i++) {
                Alert alert = new Alert();
                alert.setLocation(locations[rng.nextInt(locations.length)]);
                // 课间和课后概率更高
                int slot = weightedSlot(rng);
                int hour = timeSlots[slot][0] + rng.nextInt(timeSlots[slot][1] - timeSlots[slot][0] + 1);
                int minute = rng.nextInt(60);
                LocalDateTime alertTime = day.withHour(hour).withMinute(minute).withSecond(rng.nextInt(60));
                alert.setTime(alertTime.toString().replace("T", " ").substring(0, 19));
                alert.setCreateTime(alertTime);
                alert.setStatus("DONE");
                alert.setAlarmType(alarmTypes[rng.nextInt(alarmTypes.length)]);
                alert.setConfidence(0.78 + rng.nextDouble() * 0.17); // 0.78~0.95
                alert.setAudioLevel(65 + rng.nextInt(25));
                generated.add(alert);
            }
        }

        alertRepository.saveAll(generated);

        Map<String, Object> result = new LinkedHashMap<>();
        result.put("message", "演示数据已生成");
        result.put("count", generated.size());
        return result;
    }

    /** 时段加权：课间(40%) > 课后(25%) > 午休(15%) > 晚间(12%) > 晨间(8%) */
    private int weightedSlot(Random rng) {
        int r = rng.nextInt(100);
        if (r < 8) return 0;       // 晨间
        if (r < 23) return 1;      // 午休
        if (r < 63) return 2;      // 课间
        if (r < 88) return 3;      // 课后
        return 4;                   // 晚间
    }
}
