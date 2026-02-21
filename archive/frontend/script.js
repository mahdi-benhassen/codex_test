const testimonials = [
  {
    quote:
      "Je n'ouvre plus cinq dashboards le lundi matin. Quiet KPI me donne les décisions à prendre en 2 minutes.",
    author: 'Maya L., CEO @ Flowdesk'
  },
  {
    quote:
      "Le ton est rassurant et les alertes sont actionnables. C'est devenu notre rituel hebdomadaire d'équipe fondatrice.",
    author: 'Antoine R., cofondateur @ Atlas CRM'
  },
  {
    quote:
      "On partage le bulletin tel quel aux business angels. La clarté est incomparable face à nos anciens exports.",
    author: 'Clara T., Founder @ Looply'
  }
];

const testimonialCard = document.getElementById('testimonialCard');
const testimonialDots = document.getElementById('testimonialDots');
const prevBtn = document.getElementById('prevTestimonial');
const nextBtn = document.getElementById('nextTestimonial');

let current = 0;
let autoRotate;

function renderTestimonial(index) {
  const item = testimonials[index];
  testimonialCard.innerHTML = `<blockquote>“${item.quote}”</blockquote><cite>${item.author}</cite>`;

  [...testimonialDots.children].forEach((dot, idx) => {
    dot.classList.toggle('active', idx === index);
  });
}

function createDots() {
  testimonials.forEach((_, idx) => {
    const dot = document.createElement('button');
    dot.type = 'button';
    dot.ariaLabel = `Témoignage ${idx + 1}`;
    dot.addEventListener('click', () => {
      current = idx;
      renderTestimonial(current);
      restartAutoRotate();
    });
    testimonialDots.append(dot);
  });
}

function shift(step) {
  current = (current + step + testimonials.length) % testimonials.length;
  renderTestimonial(current);
  restartAutoRotate();
}

function restartAutoRotate() {
  clearInterval(autoRotate);
  autoRotate = setInterval(() => shift(1), 7000);
}

prevBtn?.addEventListener('click', () => shift(-1));
nextBtn?.addEventListener('click', () => shift(1));

createDots();
renderTestimonial(current);
restartAutoRotate();

const toggleButtons = document.querySelectorAll('.btn-toggle');
const prices = document.querySelectorAll('.price');

toggleButtons.forEach((button) => {
  button.addEventListener('click', () => {
    const plan = button.dataset.plan;

    toggleButtons.forEach((btn) => btn.classList.toggle('is-active', btn === button));

    prices.forEach((price) => {
      price.textContent =
        plan === 'yearly' ? price.dataset.yearly : price.dataset.monthly;
    });
  });
});

const revealElements = document.querySelectorAll('.reveal');
const observer = new IntersectionObserver(
  (entries, obs) => {
    entries.forEach((entry) => {
      if (entry.isIntersecting) {
        entry.target.classList.add('is-visible');
        obs.unobserve(entry.target);
      }
    });
  },
  {
    threshold: 0.2,
    rootMargin: '0px 0px -40px 0px'
  }
);

revealElements.forEach((el) => observer.observe(el));
