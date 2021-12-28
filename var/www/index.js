(() => {
  const T = document.getElementById('t');
  const H = document.getElementById('h');
  const HI = document.getElementById('hi');
  const R = () => window.fetch("/status")
    .then(r => r.json())
    .then(({t,h,hi}) => {
      if (t) T.textContent = t.toFixed(2);
      else console.error('Missed temperature reading!');
      
      if (h) H.textContent = h.toFixed(2);
      else console.error('Missed humidity reading!');
      
      if (hi) HI.textContent = hi.toFixed(2);
      else console.error('Missed heat index reading!');
    })
    .catch(console.error);
  R();
  setInterval(R, 2000);
})();
