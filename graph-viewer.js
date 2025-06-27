let graphData = null;

function showStatus(message, isError = false) {
    const statusDiv = document.getElementById('status');
    statusDiv.innerHTML = message;
    statusDiv.className = 'status ' + (isError ? 'error' : 'success');
}

function updateGraphStats(data) {
    const cities = data.cities;
    const connections = data.connections || [];

    document.getElementById('graphStats').innerHTML =
        `Graph loaded: ${cities.length} cities, ${connections.length} routes`;
}

function loadFromFile() {
    const fileInput = document.getElementById('fileInput');
    const file = fileInput.files[0];

    if (!file) {
        showStatus('Please select a JSON file first.', true);
        return;
    }

    const reader = new FileReader();
    reader.onload = function (e) {
        try {
            const data = JSON.parse(e.target.result);

            if (!data.cities || !Array.isArray(data.cities)) {
                throw new Error('Invalid format: Expected "cities" array in JSON');
            }

            graphData = data;
            renderGraph(data);
            updateGraphStats(data);
            showStatus('Graph loaded successfully!');
        } catch (error) {
            showStatus('Error loading file: ' + error.message, true);
        }
    };

    reader.readAsText(file);
}

function calculateGraphBounds(cities) {
    let minX = Infinity, maxX = -Infinity, minY = Infinity, maxY = -Infinity;
    cities.forEach(city => {
        minX = Math.min(minX, city.x);
        maxX = Math.max(maxX, city.x);
        minY = Math.min(minY, city.y);
        maxY = Math.max(maxY, city.y);
    });
    return { minX, maxX, minY, maxY };
}

function calculateScaleAndDimensions(bounds, containerWidth, containerHeight) {
    const padding = 50;
    const availableWidth = containerWidth - padding * 2;
    const availableHeight = containerHeight - padding * 2;

    const dataWidth = bounds.maxX - bounds.minX;
    const dataHeight = bounds.maxY - bounds.minY;

    const scaleX = availableWidth / dataWidth;
    const scaleY = availableHeight / dataHeight;
    const scale = Math.min(scaleX, scaleY, 3); // Cap at 3x for readability

    const width = dataWidth * scale + padding * 2;
    const height = dataHeight * scale + padding * 2;

    return { scale, width, height, padding };
}

function createCityMap(cities) {
    const cityMap = {};
    cities.forEach(city => {
        cityMap[city.name] = { x: city.x, y: city.y };
    });
    return cityMap;
}

function groupConnectionsByCityPairs(connections) {
    const connectionGroups = {};
    connections.forEach(conn => {
        const key = `${conn.city1}-${conn.city2}`;
        if (!connectionGroups[key]) {
            connectionGroups[key] = [];
        }
        connectionGroups[key].push(conn);
    });
    return connectionGroups;
}

function createTransformFunctions(bounds, scale, padding) {
    return {
        transformX: (x) => (x - bounds.minX) * scale + padding,
        transformY: (y) => (y - bounds.minY) * scale + padding
    };
}

function drawEdge(svg, city1, city2, conn, index, groupLength, transformX, transformY) {
    const x1 = transformX(city1.x);
    const y1 = transformY(city1.y);
    const x2 = transformX(city2.x);
    const y2 = transformY(city2.y);

    const offset = (groupLength > 1) ? (index - (groupLength - 1) / 2) * 8 : 0;

    // Calculate perpendicular vector for offset
    const dx = x2 - x1;
    const dy = y2 - y1;
    const length = Math.sqrt(dx * dx + dy * dy);

    if (length > 0) {
        const perpX = -dy / length * offset;
        const perpY = dx / length * offset;

        const offsetX1 = x1 + perpX;
        const offsetY1 = y1 + perpY;
        const offsetX2 = x2 + perpX;
        const offsetY2 = y2 + perpY;

        // Draw gray border (thick line) - dashed
        const borderLine = document.createElementNS('http://www.w3.org/2000/svg', 'line');
        borderLine.setAttribute('x1', offsetX1);
        borderLine.setAttribute('y1', offsetY1);
        borderLine.setAttribute('x2', offsetX2);
        borderLine.setAttribute('y2', offsetY2);
        borderLine.setAttribute('stroke', 'gray');
        borderLine.setAttribute('stroke-width', 6);
        borderLine.setAttribute('stroke-linecap', 'round');
        borderLine.setAttribute('stroke-dasharray', '10,5');
        borderLine.setAttribute('fill', 'none');
        svg.appendChild(borderLine);

        // Draw colored fill (thinner line on top) - dashed
        const fillLine = document.createElementNS('http://www.w3.org/2000/svg', 'line');
        fillLine.setAttribute('x1', offsetX1);
        fillLine.setAttribute('y1', offsetY1);
        fillLine.setAttribute('x2', offsetX2);
        fillLine.setAttribute('y2', offsetY2);
        fillLine.setAttribute('stroke', conn.color);
        fillLine.setAttribute('stroke-width', 4);
        fillLine.setAttribute('stroke-linecap', 'round');
        fillLine.setAttribute('stroke-dasharray', '8,4');
        fillLine.setAttribute('fill', 'none');
        fillLine.setAttribute('opacity', '0.8');
        svg.appendChild(fillLine);
    }
}

function drawCostCircle(svg, city1, city2, cost, transformX, transformY) {
    const x1 = transformX(city1.x);
    const y1 = transformY(city1.y);
    const x2 = transformX(city2.x);
    const y2 = transformY(city2.y);

    // Calculate midpoint for cost circle
    const midX = (x1 + x2) / 2;
    const midY = (y1 + y2) / 2;

    // Draw cost circle
    const costCircle = document.createElementNS('http://www.w3.org/2000/svg', 'circle');
    costCircle.setAttribute('cx', midX);
    costCircle.setAttribute('cy', midY);
    costCircle.setAttribute('r', 12);
    costCircle.setAttribute('fill', 'white');
    costCircle.setAttribute('stroke', 'black');
    costCircle.setAttribute('stroke-width', 2);
    svg.appendChild(costCircle);

    // Draw cost text
    const costText = document.createElementNS('http://www.w3.org/2000/svg', 'text');
    costText.setAttribute('x', midX);
    costText.setAttribute('y', midY);
    costText.setAttribute('class', 'edge-cost');
    costText.textContent = cost;
    svg.appendChild(costText);
}

function splitCityNameIntoLines(cityName) {
    const words = cityName.split(' ');
    if (words.length === 2) {
        return [words[0], words[1]];
    } else {
        // Break into lines based on character length
        let lines = [];
        let currentLine = '';
        const maxLineLength = 12;
        
        words.forEach((word) => {
            if (currentLine === '') {
                currentLine = word;
            } else if ((currentLine + ' ' + word).length <= maxLineLength) {
                currentLine += ' ' + word;
            } else {
                lines.push(currentLine);
                currentLine = word;
            }
        });
        if (currentLine) {
            lines.push(currentLine);
        }
        return lines;
    }
}

function calculateCityCircleRadius(lines) {
    const longestLine = lines.reduce((max, line) => line.length > max.length ? line : max, '');
    return Math.max(16, longestLine.length * 2.7 + lines.length * 1.75);
}

function drawCity(svg, city, transformX, transformY) {
    const x = transformX(city.x);
    const y = transformY(city.y);

    const lines = splitCityNameIntoLines(city.name);
    const radius = calculateCityCircleRadius(lines);

    // Draw city circle
    const circle = document.createElementNS('http://www.w3.org/2000/svg', 'circle');
    circle.setAttribute('cx', x);
    circle.setAttribute('cy', y);
    circle.setAttribute('r', radius);
    circle.setAttribute('class', 'city-circle');
    svg.appendChild(circle);

    // Draw city label with multiple lines
    const lineHeight = 12;
    const totalHeight = (lines.length - 1) * lineHeight;
    const startY = y - totalHeight / 2;

    lines.forEach((line, index) => {
        const text = document.createElementNS('http://www.w3.org/2000/svg', 'text');
        text.setAttribute('x', x);
        text.setAttribute('y', startY + index * lineHeight);
        text.setAttribute('class', 'city-label');
        text.textContent = line;
        svg.appendChild(text);
    });
}

function drawEdges(svg, connectionGroups, cityMap, transformX, transformY) {
    Object.values(connectionGroups).forEach(group => {
        group.forEach((conn, index) => {
            const city1 = cityMap[conn.city1];
            const city2 = cityMap[conn.city2];

            if (!city1 || !city2) {
                console.warn(`City not found: ${conn.city1} or ${conn.city2}`);
                return;
            }

            drawEdge(svg, city1, city2, conn, index, group.length, transformX, transformY);
        });
    });
}

function drawCostCircles(svg, connectionGroups, cityMap, transformX, transformY) {
    Object.values(connectionGroups).forEach(group => {
        const firstConn = group[0];
        const city1 = cityMap[firstConn.city1];
        const city2 = cityMap[firstConn.city2];

        if (!city1 || !city2) {
            return;
        }

        drawCostCircle(svg, city1, city2, firstConn.cost, transformX, transformY);
    });
}

function drawCities(svg, cities, transformX, transformY) {
    cities.forEach(city => {
        drawCity(svg, city, transformX, transformY);
    });
}

function renderGraph(data) {
    const svg = document.getElementById('graphSvg');
    svg.innerHTML = ''; // Clear existing content

    const cities = data.cities;
    const connections = data.connections || [];

    // Calculate bounds and scaling
    const bounds = calculateGraphBounds(cities);
    const container = document.getElementById('graphContainer');
    const containerWidth = container.clientWidth || 1100;
    const containerHeight = container.clientHeight || 700;
    const { scale, width, height } = calculateScaleAndDimensions(bounds, containerWidth, containerHeight);

    // Set SVG dimensions
    svg.setAttribute('width', width);
    svg.setAttribute('height', height);

    // Create helper functions and data structures
    const cityMap = createCityMap(cities);
    const connectionGroups = groupConnectionsByCityPairs(connections);
    const { transformX, transformY } = createTransformFunctions(bounds, scale, 50);

    // Draw graph elements in order (edges first, then costs, then cities)
    drawEdges(svg, connectionGroups, cityMap, transformX, transformY);
    drawCostCircles(svg, connectionGroups, cityMap, transformX, transformY);
    drawCities(svg, cities, transformX, transformY);
}

// Initialize
document.addEventListener('DOMContentLoaded', function() {
    document.getElementById('fileInput').addEventListener('change', function () {
        document.getElementById('status').innerHTML = '';
    });
}); 