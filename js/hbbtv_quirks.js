(function () {
  function pro7gofreshtvpro7(event) {
    const keyCode = event.keyCode || event.which;
    if (keyCode == 13) {
      var oEvent = new MouseEvent("click", {
        bubbles: true,
        cancelable: true,
      });
      document.activeElement.dispatchEvent(oEvent);
    }
  }

  if (window.location.href.indexOf("http://pro7.gofresh.tv/pro7") > -1)
    window.addEventListener("keydown", pro7gofreshtvpro7);
})();
