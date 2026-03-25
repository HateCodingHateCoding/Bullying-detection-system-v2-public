package com.bullying.alert.entity;

import com.fasterxml.jackson.annotation.JsonFormat;
import lombok.Data;
import java.time.LocalDateTime;

@Data
public class Alert {
    private Long id;
    private String location;

    @JsonFormat(pattern = "yyyy-MM-dd HH:mm:ss")
    private String time;

    private String status;
    private Double confidence;
    private String evidenceUrl;

    @JsonFormat(pattern = "yyyy-MM-dd'T'HH:mm:ss.SSSSSSSSS")
    private LocalDateTime createTime;
}
