package com.bullying.alert.dto;

import lombok.Data;

@Data
public class IoTDAMessage {
    private String notifyType;
    private String deviceId;
    private String gatewayId;
    private String requestId;
    private String productId;
    private Service[] services;

    @Data
    public static class Service {
        private String serviceId;
        private String serviceType;
        private Properties properties;
        private String eventTime;
    }

    @Data
    public static class Properties {
        private String alarmType;
        private Double confidence;
        private Long timestamp;
        private String location;
        private Integer audioLevel;
        private String radarData;
        private String evidenceUrl;
    }
}
