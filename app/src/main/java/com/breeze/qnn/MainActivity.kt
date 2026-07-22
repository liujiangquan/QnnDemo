package com.breeze.qnn

import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import androidx.fragment.app.Fragment
import com.breeze.qnn.ui.BackendFragment
import com.breeze.qnn.ui.ChatFragment
import com.breeze.qnn.ui.SettingsFragment
import com.google.android.material.bottomnavigation.BottomNavigationView

class MainActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

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
