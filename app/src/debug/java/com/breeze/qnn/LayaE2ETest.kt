package com.breeze.qnn

import androidx.test.ext.junit.runners.AndroidJUnit4
import androidx.test.platform.app.InstrumentationRegistry
import com.breeze.qnn.laya.*
import kotlinx.coroutines.runBlocking
import org.junit.Assert.assertTrue
import org.junit.Assume.assumeTrue
import org.junit.Test
import org.junit.runner.RunWith

@RunWith(AndroidJUnit4::class)
class LayaE2ETest {
    private val ctx = InstrumentationRegistry.getInstrumentation().targetContext

    @Test fun choice_department_billing() = runBlocking {
        val be = LayaBackend(ctx)
        assumeTrue("模型未就绪，跳过（先跑 setup_laya.sh）", be.modelReady())
        assertTrue("load 失败", be.load())
        val state = """{"subject":"Refund not received","body":"I cancelled my subscription two weeks ago and still have not received my refund. If not resolved I will dispute with my bank."}"""
        val q = ChoiceQuestion(
            "Which team should handle this ticket?",
            linkedMapOf(
                "billing" to "payments, refunds, invoices",
                "support" to "product help and bugs",
                "sales" to "new purchases and upgrades",
            ),
        )
        val ans = be.decide(state, linkedMapOf("department" to q))["department"] as ChoiceAnswer
        android.util.Log.i("LayaE2ETest", "choice=${ans.choice} probs=${ans.probabilities} conf=${ans.confidence}")
        assertTrue("期望 billing，实得 ${ans.choice}", ans.choice == "billing")
        be.close()
    }

    @Test fun noul_and_score_run() = runBlocking {
        val be = LayaBackend(ctx)
        assumeTrue(be.modelReady()); assertTrue(be.load())
        val state = "The customer is furious and threatens to leave."
        val res = be.decide(state, linkedMapOf(
            "urgency" to ScoreQuestion("How urgent?", listOf("not urgent", "somewhat", "urgent", "critical")),
            "churn" to NoulQuestion("Is the customer likely to cancel?"),
        ))
        val score = res["urgency"] as ScoreAnswer
        val noul = res["churn"] as NoulAnswer
        android.util.Log.i("LayaE2ETest", "urgency=${score.score} churn=${noul.noul}")
        assertTrue(score.score in 0f..3f)
        assertTrue(noul.noul in 0f..1f)
        be.close()
    }
}
