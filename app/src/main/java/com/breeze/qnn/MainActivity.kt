package com.breeze.qnn

import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import androidx.core.view.ViewCompat
import androidx.core.view.WindowCompat
import androidx.core.view.WindowInsetsCompat
import androidx.fragment.app.Fragment
import com.breeze.qnn.ui.BackendFragment
import com.breeze.qnn.ui.ChatFragment
import com.breeze.qnn.ui.SettingsFragment
import com.google.android.material.bottomnavigation.BottomNavigationView

class MainActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        // targetSdk 36 强制 edge-to-edge —— 用 WindowInsetsListener 给 root layout
        // 补 status bar 顶部 padding + navigation bar 底部 padding，内容不被系统栏遮住。
        WindowCompat.setDecorFitsSystemWindows(window, false)
        setContentView(R.layout.activity_main)
        ViewCompat.setOnApplyWindowInsetsListener(findViewById(R.id.root)) { v, insets ->
            val bars = insets.getInsets(WindowInsetsCompat.Type.systemBars())
            v.setPadding(bars.left, bars.top, bars.right, bars.bottom)
            insets
        }

        if (savedInstanceState == null) {
            switchTo(BackendFragment())
        }

        findViewById<BottomNavigationView>(R.id.bottom_nav)
            .setOnItemSelectedListener { item ->
                when (item.itemId) {
                    R.id.nav_backend  -> switchTo(BackendFragment())
                    R.id.nav_chat     -> switchTo(ChatFragment())
                    R.id.nav_settings -> switchTo(SettingsFragment())
                    else -> return@setOnItemSelectedListener false
                }
                true
            }
    }

    private fun switchTo(f: Fragment) {
        supportFragmentManager.beginTransaction()
            .replace(R.id.fragment_container, f).commit()
    }
}
