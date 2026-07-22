# LLM Baseline 性能记录

> 8845 signed PD + Qwen3-3B（INT4 量化）在 QnnDemo 上的实测性能，
> 用于回归对比。跑 5 次取 median。

## 环境

- 设备：Snapdragon 8845 (SM8480XP)，Hexagon V81，8 MB VTCM
- ROM：StepX userdebug，dev-keys
- SDK：QAIRT 2.48.40，Genie 1.19.0
- 模型：Qwen3-3B INT4 (context binary 编译时 soc=SM8480XP, VTCM=8MB)

## Baseline 数据（YYYY-MM-DD，待填）

| 指标 | 目标 | 实测 median | 实测 range |
|---|---|---|---|
| loadModel 时间 | ≤ 5 秒 | — | — |
| First token latency | ≤ 500 ms | — | — |
| Steady-state tokens/s | ≥ 15 | — | — |
| prefill tokens/s (input=512) | ≥ 100 | — | — |

## 复现方法

```bash
# 首次接机器 push 权重（外部工作已就绪）
bash docs/setup_qwen3_llm.sh <path-to-qwen3-3b-package>

# 跑 5 次取 median
for i in 1 2 3 4 5; do
    bash docs/run_e2e_tests.sh llmGenerate 2>&1 | grep -aoE 'speed=[0-9.]+ tok/s'
done
```

## 版本 / 变更历史

- YYYY-MM-DD：初次填入，llm-qwen3-8845 分支合并前的 baseline

## 相关文档

- `docs/superpowers/specs/2026-07-22-8845-llm-inference-design.md` — Design spec
- `docs/superpowers/plans/2026-07-22-8845-llm-qwen3.md` — Implementation plan
- `docs/setup_qwen3_llm.sh` — 权重预置脚本
- `docs/QNN集成完整指南.md` — QNN 基础集成流程
