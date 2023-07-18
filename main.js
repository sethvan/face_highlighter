const gl = document
  .querySelector("canvas")
  .getContext("webgl2", { preserveDrawingBuffer: true });
const canvas = document.getElementById("myCanvas");
const ext = gl.getExtension("OES_element_index_uint");

//#region Imports

import { mat4 } from "https://cdn.jsdelivr.net/npm/gl-matrix@3.4.3/+esm";
import {
  mainVertexShaderSrc,
  mainFragmentShaderSrc,
} from "./shaders/default.js";
import {
  pickingVertexShaderSrc,
  pickingFragmentShaderSrc,
} from "./shaders/picking.js";
import {
  createShaderProgram,
  createVao,
  loadImage,
  generateTexture,
  keys,
  Rotator,
  saveSelection,
  Translator,
  Scalar,
  setToDefaultPosition,
} from "./funcs.js";
import { PickingTexture } from "./pickingTexture.js";

//#endregion

//#region Set up keys for moving and mouse stuff for picking
let instance = 0;
let nameInputIsFocused = false;
function handleKeyDown(event) {
  if (!nameInputIsFocused) {
    if (keys.hasOwnProperty(event.key)) {
      keys[event.key] = true;
    }
  }
}
function handleKeyUp(event) {
  if (!nameInputIsFocused) {
    if (keys.hasOwnProperty(event.key)) {
      keys[event.key] = false;
    }
  }
}
let leftMouseButtonIsPressed = false;
let cursorX = 0;
let cursorY = 0;
function handleMouseDown(event) {
  if (event && event.button === 0) {
    leftMouseButtonIsPressed = true;
    // Get the client rect of the canvas element
    let rect = canvas.getBoundingClientRect();

    // Calculate the relative position of the mouse cursor within the canvas
    cursorX = event.clientX - rect.left;
    cursorY = event.clientY - rect.top;
  }
}
function handleMouseUp(event) {
  if (event && event.button === 0) {
    leftMouseButtonIsPressed = false;
  }
}

canvas.addEventListener("mousedown", handleMouseDown);
canvas.addEventListener("mouseup", handleMouseUp);
document.addEventListener("keydown", handleKeyDown);
document.addEventListener("keyup", handleKeyUp);

//#endregion

//#region Set up Shader Programs and get Uniform locations

const mainProgram = createShaderProgram(
  gl,
  mainVertexShaderSrc,
  mainFragmentShaderSrc
);
gl.useProgram(mainProgram);
const uniformModel = gl.getUniformLocation(mainProgram, "model");
const uniformProjection = gl.getUniformLocation(mainProgram, "projection");
const uniformView = gl.getUniformLocation(mainProgram, "view");
const uniformEyePosition = gl.getUniformLocation(mainProgram, "eyePosition");

// Material uniforms
const uniformSpecularIntensity = gl.getUniformLocation(
  mainProgram,
  "material.specularIntensity"
);
const uniformShininess = gl.getUniformLocation(
  mainProgram,
  "material.shininess"
);

//Directional light uniforms
const uniformDirectionalLightAmbientColour = gl.getUniformLocation(
  mainProgram,
  "directionalLight.base.colour"
);
const uniformDirectionalLightAmbientIntensity = gl.getUniformLocation(
  mainProgram,
  "directionalLight.base.ambientIntensity"
);
const uniformDirectionalLightDiffuseIntensity = gl.getUniformLocation(
  mainProgram,
  "directionalLight.base.diffuseIntensity"
);
const uniformDirectionalLightDirection = gl.getUniformLocation(
  mainProgram,
  "directionalLight.direction"
);

//Point light uniforms
const uniformPointLightAmbientColour = gl.getUniformLocation(
  mainProgram,
  "pointLight.base.colour"
);
const uniformPointLightAmbientIntensity = gl.getUniformLocation(
  mainProgram,
  "pointLight.base.ambientIntensity"
);
const uniformPointLightDiffuseIntensity = gl.getUniformLocation(
  mainProgram,
  "pointLight.base.diffuseIntensity"
);
const uniformPointLightPosition = gl.getUniformLocation(
  mainProgram,
  "pointLight.position"
);
const uniformPointLightConstant = gl.getUniformLocation(
  mainProgram,
  "pointLight.constant"
);
const uniformPointLightLinear = gl.getUniformLocation(
  mainProgram,
  "pointLight.linear"
);
const uniformPointLightExponent = gl.getUniformLocation(
  mainProgram,
  "pointLight.exponent"
);
gl.useProgram(null);

const useLights = () => {
  gl.uniform1f(uniformSpecularIntensity, 1.0);
  gl.uniform1f(uniformShininess, 64);

  gl.uniform3f(uniformDirectionalLightAmbientColour, 1.0, 1.0, 1.0);
  gl.uniform1f(uniformDirectionalLightAmbientIntensity, 0.1);
  gl.uniform1f(uniformDirectionalLightDiffuseIntensity, 0.1);
  gl.uniform3f(uniformDirectionalLightDirection, 0.0, -1.0, 0.0);

  gl.uniform3f(uniformPointLightAmbientColour, 1.0, 1.0, 1.0);
  gl.uniform1f(uniformPointLightAmbientIntensity, 0.7);
  gl.uniform1f(uniformPointLightDiffuseIntensity, 1.0);
  gl.uniform3f(uniformPointLightPosition, 0.0, 2.0, 3.0);
  gl.uniform1f(uniformPointLightConstant, 0.3);
  gl.uniform1f(uniformPointLightLinear, 0.2);
  gl.uniform1f(uniformPointLightExponent, 0.1);
};

//Picking shader and picking uniforms
const pickingProgram = createShaderProgram(
  gl,
  pickingVertexShaderSrc,
  pickingFragmentShaderSrc
);
gl.useProgram(pickingProgram);
const pickingUniformModel = gl.getUniformLocation(pickingProgram, "model");
const pickingUniformProjection = gl.getUniformLocation(
  pickingProgram,
  "projection"
);
const pickingUniformView = gl.getUniformLocation(pickingProgram, "view");

gl.useProgram(null);

//#endregion

//#region Set up inputs and textures

const fileInput = document.getElementById("file-input");
const toleranceInput = document.getElementById("tolerance");
const resetSelectionButton = document.getElementById("reset-selection");
const saveSelectionButton = document.getElementById("save-selection-button");
const selectionNameInput = document.getElementById("selection-name-input");
const displaySelectionButton = document.getElementById(
  "selection-dropdown-button"
);
const selectionDropdown = document.getElementById("selection-dropdown");
const defaultPositionButton = document.getElementById(
  "default-position-button"
);

const handleInputFocus = function () {
  nameInputIsFocused = true;
};
const handleInputBlur = function () {
  nameInputIsFocused = false;
};
selectionNameInput.addEventListener("focus", handleInputFocus);
selectionNameInput.addEventListener("blur", handleInputBlur);
toleranceInput.addEventListener("focus", handleInputFocus);
toleranceInput.addEventListener("blur", handleInputBlur);

defaultPositionButton.addEventListener("click", setToDefaultPosition);

const mainTexImage = await loadImage("../res/textures/Solid_silver.png");
const mainTexture = generateTexture(gl, mainTexImage, 200, 200);
const selectionTexImage = await loadImage("../res/textures/yellow.png");
const selectionTexture = generateTexture(gl, selectionTexImage, 1200, 900);
gl.clearColor(0.34, 0.425, 0.6, 5);
gl.clear(gl.DEPTH_BUFFER_BIT | gl.COLOR_BUFFER_BIT);

//#endregion

//#region Load and render STL file
fileInput.addEventListener("click", function (event) {
  if (instance++) {
    alert("Please refresh page before loading another STL file");
    event.preventDefault();
    return;
  }
});

fileInput.addEventListener("change", async (event) => {
  const file = event.target.files[0];
  const reader = new FileReader();

  reader.onload = async (event) => {
    const fileContent = event.target.result;
    const fileContentAsArray = new Uint8Array(fileContent);

    try {
      // Pass file contents to C++ class constructor, pLoader loads model using assimp in emscripten
      // ..and uses glm to calculate vertices for picked faces.
      const pLoader = await new Module.PickLoader(fileContentAsArray);

      const indices = pLoader.indices;
      const vertices = pLoader.vertices;
      const center = pLoader.model_center;
      const initialScaleFactor = pLoader.scaleFactor;
      const numTriangles = pLoader.numTriangles;

      const vao = createVao(gl, indices, vertices);
      const pickingTexture = new PickingTexture();
      pickingTexture.init(gl, 1000, 800);

      setToDefaultPosition();
      let deltaTime = 0;
      let lastTime = 0;
      let selections = {};
      let faces = {};
      selectionDropdown.innerHTML = "";

      resetSelectionButton.addEventListener("click", function () {
        pLoader.clearSelection();
        faces = {};
      });

      saveSelectionButton.addEventListener("click", function () {
        const selectionName = selectionNameInput.value;
        if (selectionName.length) {
          saveSelection(selections, selectionDropdown, faces, selectionName);
        }
        selectionNameInput.value = "";
        faces = {};
        pLoader.clearSelection();
      });

      displaySelectionButton.addEventListener("click", function () {
        const selectionName = selectionDropdown.value;
        if (selectionName.length) {
          faces = selections[selectionName];
          pLoader.clearSelection();
        }
      });

      const projection = mat4.create();
      mat4.perspective(
        projection,
        45.0 / 57.29578,
        gl.canvas.width / gl.canvas.height,
        0.1,
        100.0
      );

      const draw = () => {
        requestAnimationFrame(draw);
        const now = performance.now();
        deltaTime = now - lastTime;
        lastTime = now;
        Rotator.updateRotation(deltaTime);
        Translator.updateTranslation(deltaTime);
        Scalar.updateScaling(deltaTime);

        const model = mat4.create();
        mat4.rotateY(model, model, Rotator.yChange);
        mat4.rotateX(model, model, Rotator.xChange);
        mat4.scale(model, model, [Scalar.factor, Scalar.factor, Scalar.factor]);
        mat4.scale(model, model, [
          initialScaleFactor,
          initialScaleFactor,
          initialScaleFactor,
        ]);
        mat4.translate(model, model, [-center[0], -center[1], -center[2]]);

        const view = mat4.create();
        mat4.lookAt(
          view,
          [Translator.xChange, Translator.yChange, 3],
          [Translator.xChange, Translator.yChange, 0],
          [0, 1, 0]
        );

        if (leftMouseButtonIsPressed) {
          pickingTexture.enableWriting(gl);
          gl.clearBufferuiv(gl.COLOR, 0, [0, 0, 0, 1]);
          gl.clear(gl.DEPTH_BUFFER_BIT);
          gl.useProgram(pickingProgram);
          gl.enable(gl.DEPTH_TEST);
          gl.uniformMatrix4fv(pickingUniformView, false, view);
          gl.uniformMatrix4fv(pickingUniformProjection, false, projection);
          gl.uniformMatrix4fv(pickingUniformModel, false, model);
          gl.bindVertexArray(vao);
          gl.drawElements(gl.TRIANGLES, numTriangles, gl.UNSIGNED_INT, 0);
          gl.bindVertexArray(null);
          pickingTexture.disableWriting(gl);

          const vertexId = pickingTexture.readPixel(
            gl,
            (cursorX * gl.canvas.width) / gl.canvas.clientWidth,
            gl.canvas.height -
              (cursorY * gl.canvas.height) / gl.canvas.clientHeight -
              1
          );

          if (vertexId && vertexId < indices.length) {
            faces = pLoader.calcCurrentFaces(
              vertexId,
              parseFloat(toleranceInput.value)
            );
          }
        } else {
          pLoader.on = false; // for de-selecting / re-selecting
        }

        gl.clearColor(0.34, 0.425, 0.6, 5);
        gl.clear(gl.DEPTH_BUFFER_BIT | gl.COLOR_BUFFER_BIT);

        gl.useProgram(mainProgram);
        gl.enable(gl.DEPTH_TEST);

        gl.uniformMatrix4fv(uniformView, false, view);
        gl.uniformMatrix4fv(uniformProjection, false, projection);
        gl.uniformMatrix4fv(uniformModel, false, model);
        gl.uniform3f(
          uniformEyePosition,
          Translator.xChange,
          Translator.yChange,
          3.0
        );
        useLights();

        // render picked faces
        if (faces.startIndices) {
          gl.activeTexture(gl.TEXTURE0);
          gl.bindTexture(gl.TEXTURE_2D, selectionTexture);
          let i = 0;
          gl.bindVertexArray(vao);
          for (const index of faces.startIndices) {
            gl.drawElements(
              gl.TRIANGLES,
              faces.counts[i++],
              gl.UNSIGNED_INT,
              index * 4 // because of data size, 4 is the stride
            );
          }
          gl.bindVertexArray(null);
        }

        // render model
        gl.activeTexture(gl.TEXTURE0);
        gl.bindTexture(gl.TEXTURE_2D, mainTexture);
        gl.bindVertexArray(vao);
        gl.drawElements(gl.TRIANGLES, numTriangles, gl.UNSIGNED_INT, 0);
        gl.bindVertexArray(null);
        gl.useProgram(null);
      };

      draw();
    } catch (err) {
      console.log(err);
    }
  };

  reader.readAsArrayBuffer(file);
});

//#endregion
