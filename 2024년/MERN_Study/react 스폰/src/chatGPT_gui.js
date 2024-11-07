import React, { useEffect, useRef } from 'react';

const GpuGui = () => {
  const canvasRef = useRef(null);

  useEffect(() => {
    const canvas = canvasRef.current;
    let gl = canvas.getContext('webgl'); // 상수에서 let으로 변경

    if (!gl) {
      console.error('WebGL not supported, falling back on experimental-webgl');
      gl = canvas.getContext('experimental-webgl'); // 상수에서 let으로 변경
    }

    if (!gl) {
      alert('Your browser does not support WebGL');
      return;
    }

    // 삼각형을 그리는 WebGL 셰이더 작성
    const vertexShaderSource = `
      attribute vec4 aVertexPosition;
      void main(void) {
        gl_Position = aVertexPosition;
      }
    `;

    const fragmentShaderSource = `
      void main(void) {
        gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);
      }
    `;

    const vertexShader = gl.createShader(gl.VERTEX_SHADER);
    gl.shaderSource(vertexShader, vertexShaderSource);
    gl.compileShader(vertexShader);

    const fragmentShader = gl.createShader(gl.FRAGMENT_SHADER);
    gl.shaderSource(fragmentShader, fragmentShaderSource);
    gl.compileShader(fragmentShader);

    const shaderProgram = gl.createProgram();
    gl.attachShader(shaderProgram, vertexShader);
    gl.attachShader(shaderProgram, fragmentShader);
    gl.linkProgram(shaderProgram);
    gl.useProgram(shaderProgram);

    // 삼각형의 좌표를 정의
    const vertices = [
       0.0,  1.0,
      -1.0, -1.0,
       1.0, -1.0,
    ];

    const vertexBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vertexBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vertices), gl.STATIC_DRAW);

    const vertexPosition = gl.getAttribLocation(shaderProgram, 'aVertexPosition');
    gl.vertexAttribPointer(vertexPosition, 2, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(vertexPosition);

    // WebGL 캔버스에 삼각형 그리기
    gl.clearColor(0.0, 0.0, 0.0, 1.0);
    gl.clear(gl.COLOR_BUFFER_BIT);
    gl.drawArrays(gl.TRIANGLES, 0, 3);
  }, []);

  return <canvas ref={canvasRef} width={400} height={400} />;
};

export default GpuGui;
