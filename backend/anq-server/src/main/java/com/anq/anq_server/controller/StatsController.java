package com.anq.anq_server.controller;

import com.anq.anq_server.entity.Alert;
import com.anq.anq_server.repository.AlertRepository;
import org.springframework.web.bind.annotation.*;

import java.time.LocalDate;
import java.time.LocalDateTime;
import java.time.LocalTime;
import java.util.*;
import java.util.stream.Collectors;

@RestController
@RequestMapping("/stats")
@CrossOrigin(origins = "*")
public class StatsController {

    private final AlertRepository alertRepository;

    public StatsController(AlertRepository alertRepository) {
        this.alertRepository = alertRepository;
    }

    @GetMapping
    public Map<String, Object> getStats() {
        LocalDate today = LocalDate.now();
        LocalDateTime todayStart = today.atStartOfDay();
        LocalDateTime todayEnd = today.atTime(LocalTime.MAX);
        LocalDateTime yesterdayStart = today.minusDays(1).atStartOfDay();
        LocalDateTime yesterdayEnd = today.minusDays(1).atTime(LocalTime.MAX);

        // 今日警情数
        long todayCount = alertRepository.countByCreateTimeBetween(todayStart, todayEnd);
        // 昨日警情数
        long yesterdayCount = alertRepository.countByCreateTimeBetween(yesterdayStart, yesterdayEnd);
        // 处理中数量
        long processingCount = alertRepository.countByStatus("NEW");

        // 本月所有告警（用于时段和区域统计）
        LocalDateTime monthStart = today.withDayOfMonth(1).atStartOfDay();
        List<Alert> monthAlerts = alertRepository.findByCreateTimeAfter(monthStart);

        // 时段分布
        List<Map<String, Object>> timePeriods = computeTimePeriods(monthAlerts);

        // 区域分布
        List<Map<String, Object>> locationStats = computeLocationStats(monthAlerts);

        // 设备状态（基于最近1小时是否有该位置的告警活动）
        List<Map<String, Object>> devices = computeDeviceStatus();

        Map<String, Object> result = new LinkedHashMap<>();
        result.put("todayCount", todayCount);
        result.put("yesterdayCount", yesterdayCount);
        result.put("processingCount", processingCount);
        result.put("timePeriods", timePeriods);
        result.put("locationStats", locationStats);
        result.put("devices", devices);
        return result;
    }

    /**
     * 按时段统计：晨间(6-11), 午休(12-13), 课间(14-16), 课后(17-19), 晚间(20-23)
     */
    private List<Map<String, Object>> computeTimePeriods(List<Alert> alerts) {
        int[] counts = new int[5]; // 晨间, 午休, 课间, 课后, 晚间
        String[] names = {"晨间", "午休", "课间", "课后", "晚间"};

        for (Alert alert : alerts) {
            if (alert.getCreateTime() == null) continue;
            int hour = alert.getCreateTime().getHour();
            if (hour >= 6 && hour <= 11) counts[0]++;
            else if (hour >= 12 && hour <= 13) counts[1]++;
            else if (hour >= 14 && hour <= 16) counts[2]++;
            else if (hour >= 17 && hour <= 19) counts[3]++;
            else if (hour >= 20 && hour <= 23) counts[4]++;
        }

        List<Map<String, Object>> result = new ArrayList<>();
        for (int i = 0; i < 5; i++) {
            Map<String, Object> item = new LinkedHashMap<>();
            item.put("name", names[i]);
            item.put("count", counts[i]);
            result.add(item);
        }
        return result;
    }

    /**
     * 按位置统计告警频次，取 top 5
     */
    private List<Map<String, Object>> computeLocationStats(List<Alert> alerts) {
        Map<String, Long> locationCounts = alerts.stream()
                .filter(a -> a.getLocation() != null && !a.getLocation().isBlank())
                .collect(Collectors.groupingBy(Alert::getLocation, Collectors.counting()));

        List<Map<String, Object>> result = locationCounts.entrySet().stream()
                .sorted(Map.Entry.<String, Long>comparingByValue().reversed())
                .limit(5)
                .map(entry -> {
                    Map<String, Object> item = new LinkedHashMap<>();
                    item.put("name", entry.getKey());
                    item.put("count", entry.getValue());
                    return item;
                })
                .collect(Collectors.toList());
        return result;
    }

    /**
     * 设备状态：显示所有部署点位的终端，基于最近10分钟是否有活动判断在线
     */
    private List<Map<String, Object>> computeDeviceStatus() {
        // 实际部署（或规划部署）的监控终端
        String[][] devices = {
                {"图书馆-1F-卫生间", "图书馆-1F-卫生间"},
                {"食堂后院", "食堂后院"},
                {"3号楼楼梯间", "3号楼楼梯间"},
                {"南操场入口", "南操场入口"}
        };

        LocalDateTime tenMinutesAgo = LocalDateTime.now().minusMinutes(10);
        List<Alert> recentAlerts = alertRepository.findByCreateTimeAfter(tenMinutesAgo);
        Set<String> activeLocations = recentAlerts.stream()
                .map(Alert::getLocation)
                .filter(Objects::nonNull)
                .collect(Collectors.toSet());

        List<Map<String, Object>> result = new ArrayList<>();
        for (String[] device : devices) {
            Map<String, Object> item = new LinkedHashMap<>();
            item.put("name", device[0]);
            item.put("location", device[1]);
            item.put("online", activeLocations.contains(device[1]));
            result.add(item);
        }
        return result;
    }
}
