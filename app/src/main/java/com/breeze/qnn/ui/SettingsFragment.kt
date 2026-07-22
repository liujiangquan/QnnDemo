package com.breeze.qnn.ui

import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.SeekBar
import android.widget.TextView
import androidx.fragment.app.Fragment
import com.breeze.qnn.R

class SettingsFragment : Fragment() {

    private lateinit var settings: SamplingSettings

    override fun onCreateView(inflater: LayoutInflater, container: ViewGroup?, savedInstanceState: Bundle?): View =
        inflater.inflate(R.layout.fragment_settings, container, false)

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        settings = SamplingSettings(requireContext())
        setupSlider(view, R.id.slider_temp, R.id.value_temp,
            (settings.temperature * 100).toInt(), 200) { p ->
            val v = p / 100f; settings.temperature = v; "%.2f".format(v)
        }
        setupSlider(view, R.id.slider_topp, R.id.value_topp,
            (settings.topP * 100).toInt(), 100) { p ->
            val v = p / 100f; settings.topP = v; "%.2f".format(v)
        }
        setupSlider(view, R.id.slider_max, R.id.value_max,
            settings.maxTokens, 1800) { p ->
            settings.maxTokens = p; "$p"
        }
    }

    private fun setupSlider(root: View, sliderId: Int, valueId: Int,
                            init: Int, max: Int, onChange: (Int) -> String) {
        val slider = root.findViewById<SeekBar>(sliderId)
        val label  = root.findViewById<TextView>(valueId)
        slider.max = max
        slider.progress = init
        label.text = onChange(init)
        slider.setOnSeekBarChangeListener(object : SeekBar.OnSeekBarChangeListener {
            override fun onProgressChanged(sb: SeekBar?, p: Int, fromUser: Boolean) {
                label.text = onChange(p)
            }
            override fun onStartTrackingTouch(sb: SeekBar?) {}
            override fun onStopTrackingTouch(sb: SeekBar?) {}
        })
    }
}
