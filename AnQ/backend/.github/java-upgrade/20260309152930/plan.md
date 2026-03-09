# Java Upgrade Plan

## Session Details
- **Session ID**: 20260309152930
- **Project**: alert-system
- **Location**: E:\zhuomian\Bullying\AnQ\backend
- **Created**: 2026-03-09 15:29:30
- **Branch**: appmod/java-upgrade-20260309152930
- **Baseline Commit**: b92df05

## Target Versions
- **Java**: 21 (LTS)
- **Spring Boot**: 3.2.0 (unchanged)

## Options
- **Run tests before and after the upgrade**: false (no tests exist)
- **Git Available**: true

## Guidelines
(None specified by user)

## Available Tools
| Tool | Version | Path | Notes |
|------|---------|------|-------|
| JDK 17 | 17.0.16 | C:\Users\hu\.jdk\jdk-17.0.16 | Baseline |
| JDK 21 | 21.0.8 | C:\Users\hu\.jdk\jdk-21.0.8 | Target |
| Maven | 3.9.13 | C:\Users\hu\.maven\maven-3.9.13\bin\mvn.cmd | Installed in Step 1 |

## Technology Stack
| Component | Current Version | Target Version | Compatibility | Notes |
|-----------|----------------|----------------|---------------|-------|
| Java | 17 | 21 | ✅ Compatible | Direct upgrade possible |
| Spring Boot | 3.2.0 | 3.2.0 | ✅ Compatible | Supports Java 21 |
| spring-boot-starter-web | 3.2.0 | 3.2.0 | ✅ Compatible | No changes needed |
| Lombok | optional | optional | ✅ Compatible | No version specified |

## Derived Upgrades
(None required - Spring Boot 3.2.0 already supports Java 21)

## Key Challenges
1. **Minimal Dependencies**: Simple project with only web starter and Lombok
2. **No Tests**: Project has no test code currently
3. **Recent Spring Boot**: Already on Spring Boot 3.2.0 which fully supports Java 21

## Upgrade Steps

### Step 1: Setup Environment
**Objective**: Install required JDK and Maven versions

**Changes**:
- Install JDK 17.0.16 (if not present)
- Install JDK 21.0.8
- Install Maven 3.9.13

**Verification**: 
```powershell
java -version  # Verify JDK installation
mvn -version   # Verify Maven installation
```

**JDK Used**: N/A

---

### Step 2: Setup Baseline
**Objective**: Establish baseline compilation and test results

**Changes**:
- Stash any uncommitted changes
- Compile with current JDK 17
- Document baseline results (0 tests)

**Verification**:
```powershell
$env:JAVA_HOME="C:\Users\hu\.jdk\jdk-17.0.16"
mvn clean test-compile
```

**Expected Result**: Clean compilation with 5 source files

**JDK Used**: 17.0.16

---

### Step 3: Upgrade to Java 21
**Objective**: Update project to target Java 21

**Changes**:
- Update `pom.xml`: Change `<java.version>` from 17 to 21

**Verification**:
```powershell
$env:JAVA_HOME="C:\Users\hu\.jdk\jdk-21.0.8"
mvn clean test-compile
```

**Expected Result**: Clean compilation with Java 21

**JDK Used**: 21.0.8

---

### Step 4: Final Validation
**Objective**: Verify all upgrade success criteria are met

**Changes**:
- Verify all target versions achieved
- Confirm no deferred work (TODOs)
- Verify compilation passes with Java 21
- (Tests skipped - none exist)

**Verification**:
```powershell
$env:JAVA_HOME="C:\Users\hu\.jdk\jdk-21.0.8"
mvn clean test-compile
```

**Expected Result**: All checks pass

**JDK Used**: 21.0.8

---

## Plan Review
✅ All upgrade steps are feasible and complete
✅ Direct upgrade to Java 21 is safe with Spring Boot 3.2.0
✅ No intermediate versions needed
✅ No unfixable limitations identified

## Approval
Status: **CONFIRMED** by user on 2026-03-09
