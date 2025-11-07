package com.example.gridtest

import android.content.res.Resources
import android.os.Bundle
import android.view.KeyEvent
import android.view.View
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.enableEdgeToEdge
import androidx.compose.foundation.background
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.width
import androidx.compose.material3.Surface
import androidx.compose.material3.Text
import androidx.compose.material3.TextButton
import androidx.compose.runtime.Composable
import androidx.compose.runtime.mutableStateListOf
import androidx.compose.runtime.mutableStateOf
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.example.gridtest.ui.theme.GridTestTheme
import java.lang.Thread.sleep
import kotlin.concurrent.thread
import kotlin.math.ceil
import kotlin.random.Random

fun getScreenWidth(): Float {
	return Resources.getSystem().displayMetrics.widthPixels / Resources.getSystem().displayMetrics.density
}

fun getScreenHeight(): Float {
	return Resources.getSystem().displayMetrics.heightPixels / Resources.getSystem().displayMetrics.density
}

//Gets set from C++
var width = 0
var height = 0
var showAboutPage = mutableStateOf(false)
var showTouchDpad = mutableStateOf(true)

var turnedOnGame = false
val features = mutableStateListOf<Color>()

class CompanionClass {
	companion object {
		init {
			System.loadLibrary("gridtest")
		}
		external fun init() :String
		external fun initScreenRes(h: Int, w: Int) :String
		external fun getPixelColor(x: Int, y: Int) :Int
		external fun gameLoop() :Void
		external fun goToNextLevel() :Void
		external fun isMapLocked() :Boolean
		external fun lockMap() :Void
		external fun unLockMap() :Void
		external fun buttonPress(key: Int) :Void
		external fun getCanvasWidth() :Int
		external fun getCanvasHeight() :Int
	}
}

class MainActivity : ComponentActivity() {
	override fun onKeyUp(keyCode: Int, event: KeyEvent): Boolean {
		return when (keyCode) {
			KeyEvent.KEYCODE_DPAD_UP -> {
				CompanionClass.buttonPress(0)
				true
			}
			KeyEvent.KEYCODE_DPAD_LEFT -> {
				CompanionClass.buttonPress(2)
				true
			}
			KeyEvent.KEYCODE_DPAD_RIGHT -> {
				CompanionClass.buttonPress(3)
				true
			}
			KeyEvent.KEYCODE_DPAD_DOWN -> {
				CompanionClass.buttonPress(1)
				true
			}
			else -> super.onKeyUp(keyCode, event)
		}
	}

	override fun onResume() {
		window.decorView.apply {
			// Hide both the navigation bar and the status bar.
			// SYSTEM_UI_FLAG_FULLSCREEN is only available on Android 4.1 and higher, but as
			// a general rule, you should design your app to hide the status bar whenever you
			// hide the navigation bar.
			systemUiVisibility = View.SYSTEM_UI_FLAG_HIDE_NAVIGATION or View.SYSTEM_UI_FLAG_FULLSCREEN
		}
		super.onResume()
	}

	override fun onCreate(savedInstanceState: Bundle?) {
		super.onCreate(savedInstanceState)
		enableEdgeToEdge()

		window.decorView.apply {
			// Hide both the navigation bar and the status bar.
			// SYSTEM_UI_FLAG_FULLSCREEN is only available on Android 4.1 and higher, but as
			// a general rule, you should design your app to hide the status bar whenever you
			// hide the navigation bar.
			systemUiVisibility = View.SYSTEM_UI_FLAG_HIDE_NAVIGATION or View.SYSTEM_UI_FLAG_FULLSCREEN
		}
		//Log.d("jim", getScreenWidth().toString());
		//Log.d("jim", initScreenRes(1, 1));
		//sleep(1000)

		//set height and width from C++
		width = CompanionClass.getCanvasWidth()
		height = CompanionClass.getCanvasHeight()

		//don't run game only ui
		if (false) {
			features.clear()
			for (i in 0 until width * height) {
				features.add(
					Color(
						Random.nextInt(0, 255),
						Random.nextInt(0, 255),
						Random.nextInt(0, 255),
						255
					)
				)
			}
			turnedOnGame = true
		}

		if (!turnedOnGame) {
			//Log.d("jim", init()); //INIT CPP GAME
			//Log.d("jim", getPixelColor(0, 0).toString());

			features.clear()
			for (i in 0 until width * height) {
				features.add(
					Color(
						0,
						0,
						0,
						255
					)
				)
			}

			//run the game thread
			thread {
				CompanionClass.init()
				//gameLoop()
			}

			//run the update screen thread
			thread {
				while (true) {
					if (CompanionClass.isMapLocked()) {
						sleep(1)
						continue
					}
					CompanionClass.lockMap()
					for (y in 0 until height) {
						for (x in 0 until width) {
							val colorInt: Int = CompanionClass.getPixelColor(x, y)
							val r = (colorInt shr 16) and 0xFF
							val g = (colorInt shr 8) and 0xFF
							val b = colorInt and 0xFF
							features[y * width + x] = Color(r, g, b,255)
						}
					}
					CompanionClass.unLockMap()
					sleep(10)
				}
			}
			turnedOnGame = true
		}

		setContent {
			Surface(
				/*
				onClick = {
					for (i in 0 .. 511) {
						if (i < 256) {
							features.add(Color(i,30,i,255))
						} else {
							features.add(Color(30,i-256,i-256,255))
						}
					}

					recreate()
				},
				 */
				modifier = Modifier.fillMaxSize(),
				color = Color.Black
			) {
				GridTestTheme {
					MergeScreens()
				}
			}

		}
	}
}

@Composable
fun About(modifier: Modifier = Modifier) {
	Column(
		horizontalAlignment = Alignment.CenterHorizontally,
		verticalArrangement = Arrangement.Center,
		modifier = Modifier
			.fillMaxSize()
			.clickable {showAboutPage.value = false}
			.background(Color(162, 162, 162, 230))
	) {

		Text(
			text = "This is a small test game\n\n created by\n Jim Haaland Damgaard",
			fontSize = 40.sp,
			textAlign = TextAlign.Center,
			lineHeight = 40.sp,
			modifier = Modifier,
			color = Color(66, 0, 0, 255)
		)
	}
}

@Composable
fun MergeScreens(modifier: Modifier = Modifier) {
	Box(
		modifier = Modifier
			.fillMaxSize()
		,
		propagateMinConstraints = false
	) {
		Box(
			modifier = Modifier
				.fillMaxSize()
				//.background(Color(0,255,0,50))
				.align(Alignment.TopStart)
			,
			propagateMinConstraints = false
		) {
			Greeting(
				modifier = Modifier
			)
		}

		Box(
			modifier = Modifier
				.fillMaxSize()
				//.background(Color(255,0,0,50))
				.align(Alignment.TopStart)
			,
			propagateMinConstraints = false
		) {
			ControllerScreen(
				modifier = Modifier
			)
		}

		if (showAboutPage.value) {

			Box(
				modifier = Modifier
					.fillMaxSize()
					//.background(Color(255,0,0,50))
					.align(Alignment.TopStart)
				,
			) {
				About(
					modifier = Modifier
				)
			}

		}

	}
}

@Composable
fun ControllerScreen(modifier: Modifier = Modifier) {
	Column(
		modifier = Modifier
			//.background(Color.Gray)
			.fillMaxSize()
	) {
		Spacer(Modifier.weight(1.0f))

		// bottom row
		Row(
			modifier = Modifier
				.fillMaxWidth(),
				//.background(Color.Yellow),
			horizontalArrangement = Arrangement.Center,
			verticalAlignment = Alignment.Bottom
		) {

			Spacer(Modifier.width(10.dp))

			//Text(text = "LEFT BOTTOM")
			TextButton(onClick = {CompanionClass.buttonPress(4)})
			{
				Text(
					text = "#",
					fontSize = 30.sp,
					textAlign = TextAlign.Center,
					lineHeight = 1.sp,
					modifier = Modifier
						.background(Color(75, 6, 6, 180))
						.padding(8.dp, 2.dp, 8.dp, 2.dp)
					,
					color = Color(208, 189, 189, 220)
				)
			}

			TextButton(onClick = {showAboutPage.value = true})
			{
				Text(
					text = "?",
					fontSize = 30.sp,
					textAlign = TextAlign.Center,
					lineHeight = 1.sp,
					modifier = Modifier
						.background(Color(75, 6, 6, 180))
						.padding(10.dp, 2.dp, 10.dp, 2.dp)
					,
					color = Color(208, 189, 189, 220)
				)
			}

			TextButton(onClick = {showTouchDpad.value = !showTouchDpad.value})
			{
				Text(
					text = "H",
					fontSize = 30.sp,
					textAlign = TextAlign.Center,
					lineHeight = 1.sp,
					modifier = Modifier
						.background(Color(75, 6, 6, 180))
						.padding(6.dp, 2.dp, 6.dp, 2.dp)
					,
					color = Color(208, 189, 189, 220)
				)
			}

			Spacer(Modifier.weight(1.0f))

			//Text(text = "RIGHT BOTTOM")
			if (showTouchDpad.value) {
				ControllerPanel()
			}

			Spacer(Modifier.width(10.dp))
		}

		Spacer(Modifier.height(10.dp))

	}
}

@Composable
fun ControllerPanel(modifier: Modifier = Modifier) {
	Column(
		horizontalAlignment = Alignment.CenterHorizontally,
		verticalArrangement = Arrangement.Center,
		modifier = Modifier
	) {

		TextButton(onClick = {CompanionClass.buttonPress(0)})
		{
			Text(
				text = "▲",
				fontSize = 40.sp,
				textAlign = TextAlign.Center,
				lineHeight = 1.sp,
				modifier = Modifier
					.background(Color(75, 6, 6, 180))
					.padding(10.dp, 0.dp, 10.dp, 4.dp)
				,
				color = Color(208, 189, 189, 220)
			)
		}

		Spacer(modifier = Modifier.height(0.dp))

		Row {

			TextButton(onClick = {CompanionClass.buttonPress(2)})
			{
				Text(
					text = "◄",
					fontSize = 40.sp,
					textAlign = TextAlign.Center,
					modifier = Modifier
						.background(Color(75, 6, 6, 180))
						.padding(14.dp, 2.dp, 16.dp, 2.dp)
					,
					color = Color(208, 189, 189, 220)
				)
			}

			Spacer(modifier = Modifier.width(50.dp))

			TextButton(onClick = {CompanionClass.buttonPress(3)})
			{
				Text(
					text = "►",
					fontSize = 40.sp,
					textAlign = TextAlign.Center,
					modifier = Modifier
						.background(Color(75, 6, 6, 180))
						.padding(14.dp, 2.dp, 16.dp, 2.dp)
					,
					color = Color(208, 189, 189, 220)
				)
			}
		}

		Spacer(modifier = Modifier.height(0.dp))

		TextButton(onClick = {CompanionClass.buttonPress(1)})
		{
			Text(
				text = "▼",
				fontSize = 40.sp,
				textAlign = TextAlign.Center,
				modifier = Modifier
					.background(
						Color(
							75, 6, 6, 180
						)
					)
					.padding(10.dp, 2.dp, 10.dp, 4.dp)
				,
				color = Color(208, 189, 189, 220)
			)
		}

	}

}

@Composable
fun Greeting(modifier: Modifier = Modifier) {
	Column(
		horizontalAlignment = Alignment.CenterHorizontally,
		verticalArrangement = Arrangement.Center,
		modifier = Modifier
			.fillMaxSize()
	) {
		DisplayTest()
	}
}

@Composable
fun FeatureItem(myColor: Color) {
	if (getScreenWidth() < getScreenHeight()) {
		Column(
			modifier = Modifier
				.height((ceil((getScreenWidth() / width)) - 1).dp)
				.width((ceil((getScreenWidth() / width)) - 1).dp)
				.padding(0.dp)
				.background(myColor)

		) {
		}
	} else {
		Column(
			modifier = Modifier
				.height((ceil((getScreenHeight() / height)) - 0.1).dp)
				.width((ceil((getScreenHeight() / height)) - 0.1).dp)
				.padding(0.dp)
				.background(myColor)

		) {
		}
	}
}

@Composable
fun DisplayTest(modifier: Modifier = Modifier) {
	Column (modifier = modifier) {
		for (rowId in 0 until height) {
			Row {
				for (columnId in 0 until width) {
					FeatureItem(features[(rowId * width) + columnId])
				}
			}
		}
	}
}

@Preview(showBackground = true)
@Composable
fun GreetingPreview() {
	GridTestTheme {
		Greeting()
	}
}