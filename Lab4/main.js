const controllerIP = '192.168.248.128'
const controllerPort = '8181'

function getHostsInfo() {
  fetch(`http://${controllerIP}:${controllerPort}/onos/v1/hosts`,
  {
    method: 'GET',
    headers: {
      'Authorization': 'Basic ' + btoa('onos:rocks')
    }
  })
  .then(response => response.json())
  .then(data => {
    let res = document.getElementById('hostInfo');
    res.innerHTML = ''
    const hosts = data.hosts;
    for (const host of hosts)
    {
        const hostInfo = document.createElement('div');
        const label = document.createElement('h3');
        label.innerText = `HOST ${host.ipAddresses[0][7]}`
        hostInfo.innerHTML = `
        <p>id: ${host.id}</p>  
        <p>mac: ${host.mac}</p>
        <p>IP: ${host.ipAddresses[0]}</p>`
        res.appendChild(label)
        res.appendChild(hostInfo)
    }
  })
  .catch(error => console.error('Lỗi:', error));
}
  
function getSwitchInfo() {
    fetch(`http://${controllerIP}:${controllerPort}/onos/v1/devices`,
    {
      method: 'GET',
      headers: {
        'Authorization': 'Basic ' + btoa('onos:rocks')
      }
    })
      .then(response => response.json())
      .then(data => {
        let res = document.getElementById('switchInfo');
        res.innerHTML = ''
        const switchInfo = data.devices;
        for (const sw of switchInfo)
        {
          const switchPresent = document.createElement('div')
          const label = document.createElement('h3')
          label.innerHTML = `SWITCH ${sw.id[18]}`
          switchPresent.innerHTML = `
            <p>id: ${sw.id}</p>
            <p>channelId: ${sw.annotations.channelId}</p>
            <p></p>
            <p></p>
          `
          res.appendChild(label);
          res.appendChild(switchPresent);
        }        
      })
      .catch(error => console.error('Lỗi:', error));
  }
function getFlowsInfo(){
  let input = document.getElementById('flowInput')
  let endpoint = ""
  if (input.value){
   endpoint = `/of%3A${input.value.trim().slice(3)}`
  }
  fetch(`http://${controllerIP}:${controllerPort}/onos/v1/flows${endpoint}`,
  {
    method: 'GET',
    headers: {
      'Authorization': 'Basic ' + btoa('onos:rocks')
    }
  })
  .then(response => response.json())
  .then(data => {
    let res = document.getElementById('flowInfo');
    
    res.innerHTML = ''
    const flows = data.flows;
    for (const flow of flows)
    {
        const flowInfo = document.createElement('div');
        const label = document.createElement('h3');
        label.innerText = `FLOW`
        flowInfo.innerHTML = `
        <p>id: ${flow.id}</p>  
        <p>deviceID: ${flow.deviceId}</p>
        <p>type: ${flow.treatment.instructions[0].type}</p>
        <p>port: ${flow.treatment.instructions[0].port}`
        res.appendChild(label)
        res.appendChild(flowInfo)
    }
  })
  .catch(error => console.error('Lỗi:', error));
}
  
function addFlow() {
    const deviceId = document.getElementById('js-add__deviceId').value.trim();
    const port = document.getElementById('js-add__port').value.trim();
  
    const flowData = {
      priority: 40001,
      timeout: 0,
      isPermanent: true,
      deviceId: deviceId,
      treatment: {
        instructions: [
          {
            type: 'OUTPUT',
            port: port
          }
        ]
      },
      selector: {
        criteria: [
          {
            type: 'ETH_TYPE',
            ethType: '0x800'
          }
        ]
      }
    };
    
  
    fetch(`http://${controllerIP}:${controllerPort}/onos/v1/flows/of%3A${deviceId.slice(3)}?appId=org.onosproject.core`, {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
        'Authorization': 'Basic ' + btoa('onos:rocks')
      },
      body: JSON.stringify(flowData)
    })
      .catch(error => console.error('Lỗi:', error));
}
  
function deleteFlow() {
    const deviceId = document.getElementById('js-delete__deviceId').value.trim();
    const flowId = document.getElementById('js-delete__flowId').value.trim();
    fetch(`http://${controllerIP}:${controllerPort}/onos/v1/flows/of%3A${deviceId.slice(3)}/${flowId}`, {
      method: 'DELETE',
      headers: {
        'Authorization': 'Basic ' + btoa('onos:rocks')
      }
    })
      .catch(error => console.error('Lỗi:', error));
  }

function addFlowBlockH4(){
    const deviceId = 'of:0000000000000003';
    const IP = document.getElementById('js-host__block').value.trim()
    const flowData = {
      priority: 40000,
      timeout: 0,
      isPermanent: true,
      deviceId: deviceId,
      treatment: {
        instructions: [
          {
            type: 'OUTPUT',
            port: '0'
          }
        ]
      },
      selector: {
        criteria: [
          {
            type: "ETH_TYPE",
            ethType: "0x0800"
          },
         {
           type: "IPV4_DST",
           ip: `${IP}/32`
          }
        ]
      }
    };
    console.log(JSON.stringify(flowData))
    fetch(`http://${controllerIP}:${controllerPort}/onos/v1/flows/of%3A${deviceId.slice(3)}?appId=org.onosproject.core`, 
    {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
        'Authorization': 'Basic ' + btoa('onos:rocks')
      },
      body: JSON.stringify(flowData),
    })
    .catch(error => console.error('Lỗi:', error));
  }

  function addFlowBlockH3UDP(){
    const deviceId = 'of:0000000000000003';
    const IP = document.getElementById('js-host__block-udp').value.trim()
    const flowData = {
      priority: 40000,
      timeout: 0,
      isPermanent: true,
      deviceId: deviceId,
      treatment: {
        instructions: [
          {
            type: 'OUTPUT',
            port: '0'
          }
        ]
      },
      selector: {
        criteria: [
          {
            type: "ETH_TYPE",
            ethType: "0x0800"
          },
         {
           type: "IPV4_DST",
           ip: `${IP}/32`
          },
         {
            type: "IP_PROTO", 
            protocol: '17'
          }
        ]
      }
    };
    console.log(JSON.stringify(flowData))
    fetch(`http://${controllerIP}:${controllerPort}/onos/v1/flows/of%3A${deviceId.slice(3)}?appId=org.onosproject.core`, 
    {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
        'Authorization': 'Basic ' + btoa('onos:rocks')
      },
      body: JSON.stringify(flowData),
    })
    .catch(error => console.error('Lỗi:', error));
  }