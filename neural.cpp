#include <iostream>
#include <vector>
#include <cassert> 
#include <cmath>

class Axon {
public:
   Axon(double const &weight, unsigned const &in, unsigned const &out);
   double sendOutput(double const &output) const;
   unsigned connection[2];
private:
   double m_weight;
};

Axon::Axon(double const &weight, unsigned const &in, unsigned const &out) {
   m_weight = weight;
   connection[0] = in;
   connection[1] = out;
}

double Axon::sendOutput(double const &output) const {
   return output * m_weight;
}

class Neuron {
public:
   Neuron();
   void propagate(double const &input);
   void activate(void);
   double output(void);
   void manualSet(double const &output);
private:
   double m_input;
   double m_output;
};

Neuron::Neuron() {
   m_input = 0;
}

void Neuron::propagate(double const &input) {
   m_input += input;
}

void Neuron::activate(void) {
   //using Anguita activation function
   if (m_input > 1.92033) {
      m_output = 0.96016;
      return;
   }
   if ((m_input > 0) and (m_input <= 1.92033)) {
      m_output = 0.96016 - 0.26037 * pow(m_input-1.92033, 2);
      return;
   }
   if ((m_input > -1.92033) and (m_input < 0)) {
      m_output = 0.26037 * pow(m_input + 1.92033, 2) - 0.9016;
      return;
   }
   m_output = -0.96016;
}

double Neuron::output(void) {
   return m_output;
}

void Neuron::manualSet(double const &output) {
   m_output = output;
}

class Net {
public:
   Net(std::vector<std::vector<Neuron> > const &neurons, std::vector<std::vector<Axon> > const &axons);
   void forwardFeed(std::vector<double> const &inputs);
private:
   std::vector<std::vector<Axon> > m_axons;
   std::vector<std::vector<Neuron> > m_neurons;
};

Net::Net(std::vector<std::vector<Neuron> > const &neurons, std::vector<std::vector<Axon> > const &axons) {
   m_axons = axons;
   m_neurons = neurons;
}

void Net::forwardFeed(std::vector<double> const &inputs) {
   assert(m_neurons[0].size()-1 == inputs.size());
   for (unsigned i=0; i < inputs.size(); i++) {
      m_neurons[0][i].manualSet(inputs[i]);
   }

   for (unsigned l=0; l < m_neurons.size()-1; l++) {
      for (auto const &ax: m_axons[l]) {
         Neuron &outNeuron = m_neurons[l][ax.connection[0]];
         Neuron &inNeuron = m_neurons[l+1][ax.connection[1]];
         inNeuron.propagate( ax.sendOutput( outNeuron.output() ));
      }
   
      for (unsigned n=0; n < m_neurons[l+1].size()-1; n++) {
         Neuron &nrn = m_neurons[l+1][n];
         nrn.activate();
      }
   }
   for (unsigned n=0; n < m_neurons.back().size()-1; n++) {
      std::cout<<m_neurons.back()[n].output()<<std::endl;
   }
}

int main () {
   std::vector<std::vector<Neuron> > neurons;
   std::vector<std::vector<Axon> > axons;
   std::vector<unsigned> topology {1, 50, 50, 50, 1};

   double bias = 1;
   double weight = -1;

   for (unsigned l = 0; l < topology.size(); l++) {
      axons.push_back({});
      neurons.push_back({});

      for (unsigned n = 0; n < topology[l]+1; n++) {
         neurons.back().push_back(Neuron());         
         for (unsigned t = 0; t < topology[l+1]; t++) {

            if (n == topology[l]) {
               axons.back().push_back(Axon(bias, n, t));
            }
            else {
               axons.back().push_back(Axon(weight, n, t));
            }
         }
      }
      neurons.back().back().manualSet(1);
   }

   Net net {neurons, axons};
   net.forwardFeed({1});
}
