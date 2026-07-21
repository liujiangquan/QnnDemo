pluginManagement {
    repositories {
        google {
            content {
                includeGroupByRegex("com\\.android.*")
                includeGroupByRegex("com\\.google.*")
                includeGroupByRegex("androidx.*")
            }
        }
        // 内网 Nexus——托管统签 Gradle 插件 com.stepos.unifiedsigning.signing
        maven {
            isAllowInsecureProtocol = true
            url = uri("http://nexus.zyql.com/repository/maven-public/")
            credentials {
                username = "maven_ro"
                password = "aZ8nP63#kR8d"
            }
        }
        maven {
            isAllowInsecureProtocol = true
            url = uri("http://nexus.zyql.com/repository/maven-snapshots/")
            credentials {
                username = System.getenv("NEXUS_USERNAME") ?: providers.gradleProperty("nexus.username").orNull
                password = System.getenv("NEXUS_PASSWORD") ?: providers.gradleProperty("nexus.password").orNull
            }
        }
        mavenCentral()
        gradlePluginPortal()
    }
}
plugins {
    id("org.gradle.toolchains.foojay-resolver-convention") version "1.0.0"
}
dependencyResolutionManagement {
    repositoriesMode.set(RepositoriesMode.FAIL_ON_PROJECT_REPOS)
    repositories {
        google()
        mavenCentral()
        maven {
            isAllowInsecureProtocol = true
            url = uri("http://nexus.zyql.com/repository/maven-public/")
            credentials {
                username = "maven_ro"
                password = "aZ8nP63#kR8d"
            }
        }
    }
}

rootProject.name = "QnnDemo"
include(":app")
