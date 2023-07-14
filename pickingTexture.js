class PickingTexture {
  init(gl, width, height) {
    this.fbo = gl.createFramebuffer();
    gl.bindFramebuffer(gl.FRAMEBUFFER, this.fbo);

    this.pickingTexture = gl.createTexture();
    gl.bindTexture(gl.TEXTURE_2D, this.pickingTexture);
    gl.texImage2D(
      gl.TEXTURE_2D,
      0,
      gl.RGBA32UI,
      width,
      height,
      0,
      gl.RGBA_INTEGER,
      gl.UNSIGNED_INT,
      null
    );
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);
    gl.framebufferTexture2D(
      gl.FRAMEBUFFER,
      gl.COLOR_ATTACHMENT0,
      gl.TEXTURE_2D,
      this.pickingTexture,
      0
    );

    this.depthTexture = gl.createTexture();
    gl.bindTexture(gl.TEXTURE_2D, this.depthTexture);
    gl.texImage2D(
      gl.TEXTURE_2D,
      0,
      gl.DEPTH_COMPONENT24,
      width,
      height,
      0,
      gl.DEPTH_COMPONENT,
      gl.UNSIGNED_INT,
      null
    );
    gl.framebufferTexture2D(
      gl.FRAMEBUFFER,
      gl.DEPTH_ATTACHMENT,
      gl.TEXTURE_2D,
      this.depthTexture,
      0
    );

    const status = gl.checkFramebufferStatus(gl.FRAMEBUFFER);
    if (status !== gl.FRAMEBUFFER_COMPLETE) {
      console.error("FB error, status:", status);
      // Handle the error appropriately
      // For example, you might throw an error or exit the application
    }

    gl.bindTexture(gl.TEXTURE_2D, null);
    gl.bindFramebuffer(gl.FRAMEBUFFER, null);
  }

  enableWriting(gl) {
    gl.bindFramebuffer(gl.DRAW_FRAMEBUFFER, this.fbo);
  }

  disableWriting(gl) {
    gl.bindFramebuffer(gl.DRAW_FRAMEBUFFER, null);
  }

  readPixel(gl, x, y) {
    gl.bindFramebuffer(gl.READ_FRAMEBUFFER, this.fbo);
    gl.readBuffer(gl.COLOR_ATTACHMENT0);

    const pixel = new Uint32Array(4); // Assuming you want to read a single RGB pixel
    gl.readPixels(x, y, 1, 1, gl.RGBA_INTEGER, gl.UNSIGNED_INT, pixel);

    gl.readBuffer(gl.NONE);
    gl.bindFramebuffer(gl.READ_FRAMEBUFFER, null);

    return pixel[0];
  }
}

export { PickingTexture };
