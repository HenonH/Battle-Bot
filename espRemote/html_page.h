#ifndef HTML_PAGE_H
#define HTML_PAGE_H

const String htmlPage = R"=====( 
<!DOCTYPE html>
<html>
<head>
  <title>Rhoomba Remote</title>
  <style>
    body { background: #DC143C; color: white; text-align: center; margin: 0; padding: 0; overflow: hidden; font-family: Arial, sans-serif; }
    h1 { margin-top: 10px; }
    #container { display: flex; justify-content: space-around; align-items: center; height: 100vh; }
    #joystick-container { width: 300px; height: 300px; background: white; border-radius: 50%; position: relative; touch-action: none; }
    #joystick { width: 75px; height: 75px; background: #ff4d4d; border-radius: 50%; position: absolute; top: 112.5px; left: 112.5px; }
    #slider { width: 300px; }
    button { margin-top: 20px; padding: 10px 20px; font-size: 16px; background: #4CAF50; color: white; border: none; cursor: pointer; }
    button:active { background: #3e8e41; }
  </style>
</head>
<body>
  <h1>Rhoomba Remote</h1>
  <div id="container">
    <div>
      <div id="joystick-container">
        <div id="joystick"></div>
      </div>
      <p>X: <span id="xValue">50</span>, Y: <span id="yValue">50</span></p>
      <button onclick="toggleControl()">Toggle Control</button>
    </div>
    <div>
      <label>Control:</label>
      <input type="range" id="slider" min="0" max="100" value="0">
      <p>Slider: <span id="sliderValue">0</span></p>
    </div>
  </div>
  <script>
    const joystick = document.getElementById('joystick');
    const container = document.getElementById('joystick-container');
    const slider = document.getElementById('slider');
    let xValue = 50, yValue = 50, sliderValue = 0, joystickEnabled = true;
    let lastX = 50, lastY = 50, lastSlider = 0;

    container.addEventListener('touchmove', (event) => {
      if (!joystickEnabled) return;
      event.preventDefault();
      const rect = container.getBoundingClientRect();
      const centerX = rect.left + rect.width / 2;
      const centerY = rect.top + rect.height / 2;
      let x = event.touches[0].clientX - centerX;
      let y = event.touches[0].clientY - centerY;
      const distance = Math.min(Math.sqrt(x * x + y * y), 112.5);
      const angle = Math.atan2(y, x);
      x = distance * Math.cos(angle);
      y = distance * Math.sin(angle);
      joystick.style.left = `${x + 112.5}px`;
      joystick.style.top = `${y + 112.5}px`;
      xValue = Math.round((x / 112.5) * 50 + 50);
      yValue = Math.round(50 - (y / 112.5) * 50);
      document.getElementById('xValue').textContent = xValue;
      document.getElementById('yValue').textContent = yValue;
    });

    container.addEventListener('touchend', () => {
      joystick.style.left = '112.5px';
      joystick.style.top = '112.5px';
      xValue = yValue = 50;
      document.getElementById('xValue').textContent = xValue;
      document.getElementById('yValue').textContent = yValue;
    });

    slider.oninput = () => {
      sliderValue = slider.value;
      document.getElementById('sliderValue').textContent = sliderValue;
    };

    function toggleControl() {
      joystickEnabled = !joystickEnabled;
      alert(`Joystick Control ${joystickEnabled ? 'Enabled' : 'Disabled'}`);
    }

    function sendData() {
      if (xValue !== lastX || yValue !== lastY || sliderValue !== lastSlider) {
        const xhr = new XMLHttpRequest();
        xhr.open('GET', `/u?x=${xValue}&y=${yValue}&s=${sliderValue}`, true);
        xhr.send();
        lastX = xValue; lastY = yValue; lastSlider = sliderValue;
      }
    }

    setInterval(sendData, 100);
  </script>
</body>
</html>
)=====";

#endif
