package com.breeze.qnn

import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import androidx.fragment.app.Fragment
import com.breeze.qnn.databinding.ActivityMainBinding
import com.breeze.qnn.ui.BackendFragment
import com.breeze.qnn.ui.LayaFragment

class MainActivity : AppCompatActivity() {
    private lateinit var binding: ActivityMainBinding
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)
        if (savedInstanceState == null) switchTo(BackendFragment())
        binding.bottomNav.setOnItemSelectedListener {
            when (it.itemId) {
                R.id.nav_backend -> { switchTo(BackendFragment()); true }
                R.id.nav_laya -> { switchTo(LayaFragment()); true }
                else -> false
            }
        }
    }
    private fun switchTo(f: Fragment) {
        supportFragmentManager.beginTransaction().replace(R.id.fragmentContainer, f).commit()
    }
}
