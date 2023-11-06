from mininet.node import Host, OVSSwitch, Controller
from mininet.link import Link

def create_tree(n):
	#First layer is core n = 0
	#Last layer is host  n
	#Other layer is switch 1->n-1 

	#create host and setIP
	# we have 2**n host (last layer)
	host_list = []
	for i in range(2**n):
		host = Host('h%d' % (i+1))
		host_list.append(host) #host name h1 h2 h3 h4....
		
	#create controller => n = 0
	c0 = Controller('c0', inNamespace=False)

	#create switch
	switch_list = []
	for i in range(0, n): #i=0-> i=n-1 each layer
		# number of node is 2^layer
		# in layer 0, have 2^0 = 1 switch
		# in layer 1, have 2^1 = 2 switch
		# in layer 2, have 2^2 = 4 switch
		# j is order of switch in layer
		for j in range(1, 2**i + 1): #start at 1 for mask switch_name form 1
			if i == 0:
				switch_name = 'c'+ str(j) #core => layer 0
			else:
				if i == 1:
					switch_name = 'a'+ str(j) #aggregation => layer 1
				else:
					switch_name = 'e'+ str(j) #edge => other layer
			switch = OVSSwitch(switch_name, inNamespace=False)
			switch_list.append(switch)

	#create link
	#merge host_list and switch_list in to node_list
	#type of elements of node_list is Host or OVSSwitch
	node_list = switch_list + host_list
	
	#link_target is switch on upper layer which switch/host in lower layer want to Link()
	#link_target start at 0 map with node_list
	link_target = 0 
	
	#i start at 1 because first node is c1 (core)
	#try to understand the function calclating link_target yourself
	
	print('Link in tree:')
	for i in range(1, len(node_list)):
		if i == 1:
			link_target = 0
		else:
			if i%2 == 0:
				link_target = int((i-2)/2)
			else:
				link_target = int((i-1)/2)
		Link(node_list[i], node_list[link_target])
		print(node_list[link_target], '->', node_list[i])
	
	#start
	c0.start()
	for i in range(len(switch_list)):
		switch_list[i].start([c0])
		
	#set IP
	#IP start at .1
	for i in range(len(host_list)):
		host_list[i].setIP('10.0.0.%d/24' % (i+1))
	
	#print host IP
	print('Host IP:')
	for i in range(len(host_list)):
		print(host_list[i],': ',host_list[i].IP)


	#Ping node-node
	#for i in range (len(host_list)):
		#print('PING from h%d to...' % (i+1))
		#for j in range(len(host_list)):
			#if i!=j:
				#print(host_list[i].cmd('ping -c3 ', host_list[j].IP()))
	
	#stop
	for i in range(len(switch_list)):
		switch_list[i].stop()
	c0.stop()

if __name__ == "__main__":
	create_tree(3)